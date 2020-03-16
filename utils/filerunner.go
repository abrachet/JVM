package main

import (
	"bufio"
	"errors"
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"os/exec"
	"strings"
)

type ProcessStatus struct {
	Status *os.ProcessState
	Stdout string
	Stderr string
}

func (p *ProcessStatus) Successful() bool {
	if p.Status == nil {
		return false
	}
	return p.Status.ExitCode() == 0
}

func RunCommand(command string) (*ProcessStatus, error) {
	cmd := exec.Command("sh", "-c", command)
	stdoutPipe, err := cmd.StdoutPipe()
	if err != nil {
		return nil, err
	}
	stderrPipe, err := cmd.StderrPipe()
	if err != nil {
		return nil, err
	}
	if err := cmd.Start(); err != nil {
		return nil, err
	}
	stdout, err := ioutil.ReadAll(stdoutPipe)
	if err != nil {
		return nil, err
	}
	stderr, err := ioutil.ReadAll(stderrPipe)
	if err != nil {
		return nil, err
	}
	_ = cmd.Wait()
	return &ProcessStatus{cmd.ProcessState, strings.TrimSuffix(string(stdout), "\n"), strings.TrimSuffix(string(stderr), "\n")}, nil
}

type CommandType int

const (
	Require CommandType = iota
	Run
	Assert
	None
)

type Command struct {
	Type   CommandType
	Status *ProcessStatus
}

type CommandRunner struct {
	filename    string
	tempfile    string
	parsedLine  string
	currentLine int
}

func (cr *CommandRunner) replaceVariables(line string, prevStatus *ProcessStatus) string {
	line = strings.ReplaceAll(line, "%{file}", cr.filename)
	line = strings.ReplaceAll(line, "%{temp}", cr.tempfile)
	if prevStatus != nil {
		line = strings.ReplaceAll(line, "%{status}", fmt.Sprintf("%d", prevStatus.Status.ExitCode()))
		line = strings.ReplaceAll(line, "%{stderr}", fmt.Sprintf("\"%s\"", prevStatus.Stderr))
		line = strings.ReplaceAll(line, "%{stdout}", fmt.Sprintf("\"%s\"", prevStatus.Stdout))
	}
	return line
}

func (cr *CommandRunner) parseStart(line string, expect string, prevStatus *ProcessStatus) (*ProcessStatus, error) {
	if index := strings.Index(line, expect); index != -1 {
		replaced := cr.replaceVariables(line, prevStatus)
		cr.parsedLine = replaced
		return RunCommand(replaced[index+len(expect)+1:])
	}
	return nil, nil
}

func (cr *CommandRunner) parseOne(line string, prevStatus *ProcessStatus) (*Command, error) {
	status, err := cr.parseStart(line, "REQUIRE", prevStatus)
	if err != nil {
		return nil, err
	}
	if status != nil {
		return &Command{Require, status}, nil
	}

	status, err = cr.parseStart(line, "RUN", prevStatus)
	if err != nil {
		return nil, err
	}
	if status != nil {
		return &Command{Run, status}, nil
	}

	status, err = cr.parseStart(line, "ASSERT", prevStatus)
	if err != nil {
		return nil, err
	}
	if status != nil {
		return &Command{Assert, status}, nil
	}

	return &Command{None, nil}, nil
}

func (cr *CommandRunner) ParseAll() error {
	file, err := os.Open(cr.filename)
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()

	prevCommand := &Command{Run, nil}

	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := scanner.Text()
		prevCommand, err = cr.parseOne(line, prevCommand.Status)
		if err != nil {
			return err
		}
		if prevCommand == nil {
			log.Fatal("prev was nil")
		}
		if prevCommand.Type == Require && !prevCommand.Status.Successful() {
			fmt.Fprintf(os.Stderr, "Requirement failed, line %d: %s\n", cr.currentLine, cr.parsedLine)
			fmt.Fprintln(os.Stderr, "Skipping test:", cr.filename)
			return nil
		}
		if prevCommand.Type == Assert && !prevCommand.Status.Successful() {
			fmt.Fprintf(os.Stderr, "Assertion failed, line %d: %s\n", cr.currentLine, cr.parsedLine)
			fmt.Fprintf(os.Stderr, "exited with exit code %d\n", prevCommand.Status.Status.ExitCode())
			if stderr := prevCommand.Status.Stderr; len(stderr) > 0 {
				fmt.Fprintln(os.Stderr, "stderr:", stderr)
			}
			return errors.New("assertion failure")
		}
		cr.currentLine++
	}
	return nil
}

func (cr *CommandRunner) Cleanup() {
	os.Remove(cr.tempfile)
}

func NewCommandRunner(filename string) *CommandRunner {
	tmpfile, err := ioutil.TempFile("", fmt.Sprintf(".filerunner.%s.*", filename))
	if err != nil {
		log.Fatal(err)
	}
	return &CommandRunner{filename, tmpfile.Name(), "", 1}
}

func filerunner() int {
	filename := os.Args[1]
	if index := strings.LastIndexByte(filename, '/'); index != -1 {
		err := os.Chdir(filename[:index])
		if err != nil {
			log.Fatal(err)
		}
		filename = filename[index+1:]
	}
	commandRunner := NewCommandRunner(filename)
	defer commandRunner.Cleanup()
	err := commandRunner.ParseAll()
	if err != nil {
		fmt.Fprintln(os.Stderr, "\033[1;31m[   FAIL   ]\033[0m", os.Args[1])
		return 1
	}
	fmt.Fprintln(os.Stderr, "\033[1;32m[   PASS   ]\033[0m", os.Args[1])
	return 0
}

func main() {
	os.Exit(filerunner())
}
