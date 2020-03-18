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
	"time"
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

type Define struct {
	variable   string
	definition string
}

type CommandRunner struct {
	filename    string
	definitions []Define
	tempfile    string
	parsedLine  string
	currentLine int
}

func (cr *CommandRunner) replaceVariables(line string, prevStatus *ProcessStatus) string {
	line = strings.ReplaceAll(line, "%{file}", cr.filename)
	line = strings.ReplaceAll(line, "%{temp}", cr.tempfile)
	for _, defn := range cr.definitions {
		replace := fmt.Sprintf("\"%s\"", defn.definition)
		format := fmt.Sprintf("%%{%s}", defn.variable)
		line = strings.ReplaceAll(line, format, replace)
	}
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

func NewCommandRunner(filename string, defines []Define) *CommandRunner {
	tmpfile, err := ioutil.TempFile("", fmt.Sprintf(".filerunner.%s.*", filename))
	if err != nil {
		log.Fatal(err)
	}
	return &CommandRunner{filename, defines, tmpfile.Name(), "", 1}
}

func filerunner() int {
	// TODO: allow reading from stdin.
	filename := "-"
	var defines []Define
	for _, str := range os.Args[1:] {
		if !strings.HasPrefix(str, "-D") {
			filename = str
			continue
		}
		sub := strings.Split(str[2:], "=")
		if len(sub) != 2 {
			fmt.Fprintln(os.Stderr, "Error parsing:", str)
			return 1
		}
		defines = append(defines, Define{sub[0], sub[1]})
	}

	if index := strings.LastIndexByte(filename, '/'); index != -1 {
		err := os.Chdir(filename[:index])
		if err != nil {
			log.Fatal(err)
		}
		filename = filename[index+1:]
	}
	start := time.Now()
	commandRunner := NewCommandRunner(filename, defines)
	defer commandRunner.Cleanup()
	err := commandRunner.ParseAll()
	elapsed := time.Now().Sub(start)
	if err != nil {
		fmt.Fprint(os.Stderr, "\033[1;31m[   FAIL   ]\033[0m ", filename)
		fmt.Fprintf(os.Stderr, " (%dms)\n", elapsed.Milliseconds())
		return 1
	}
	fmt.Fprint(os.Stderr, "\033[1;32m[   PASS   ]\033[0m ", filename)
	fmt.Fprintf(os.Stderr, " (%dms)\n", elapsed.Milliseconds())
	return 0
}

func main() {
	os.Exit(filerunner())
}
