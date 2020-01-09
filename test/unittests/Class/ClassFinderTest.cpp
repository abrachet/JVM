
#include "JVM/Class/ClassFinder.h"
#include "gtest/gtest.h"
#include <cstdlib>
#include <fstream>

#include <fcntl.h>

TEST(ClassFinder, RegisterFromJar) {
  int exitCode;
  std::vector<std::string> entries;
  std::string err;
  if (::open("entry1", O_CREAT, 0666) == -1)
    goto removeEntries;
  if (::open("entry2", O_CREAT, 0666) == -1)
    goto removeEntries;
  if (::open("entry3", O_CREAT, 0666) == -1)
    goto removeEntries;
  // Use zip(1) instead of jar(1) because jar creates some extra files in the
  // archive.
  exitCode = std::system("zip Test.jar entry1 entry2 entry3 > /dev/null");
  if (exitCode == -1)
    goto cleanup;

  ASSERT_EQ(exitCode, 0);

  err = registerFromJar("Test.jar", [&entries](std::string entry) {
    entries.push_back(std::move(entry));
  });
  ASSERT_TRUE(err.empty()) << err;

  ASSERT_EQ(entries.size(), 3);
  EXPECT_EQ(entries[0], "entry1");
  EXPECT_STREQ(entries[1].c_str(), "entry2");
  EXPECT_STREQ(entries[2].c_str(), "entry3");

cleanup:
  (void)::remove("Test.jar");
removeEntries:
  (void)::remove("entry1");
  (void)::remove("entry2");
  (void)::remove("entry3");
}

TEST(ClassFinder, FindRTJar) {
  if (::mkdir("jre", 0644))
    return;
  if (::mkdir("jre/lib", 0644)) {
    ::rmdir("jre");
    return;
  }
  if (::open("jre/lib/rt.jar", O_CREAT) == -1) {
    ::rmdir("jre/lib");
    ::rmdir("jre");
    return;
  }
  const char *pwd = std::getenv("PWD");
  ASSERT_TRUE(pwd);
  const char *oldJHome = std::getenv("JAVA_HOME");
  setenv("JAVA_HOME", pwd, true);
  std::string path;
  std::string err = findRTJar(path);
  ASSERT_TRUE(err.empty());
  setenv("JAVA_HOME", oldJHome, true);
  ASSERT_EQ(path, std::string(pwd) + "/jre/lib/rt.jar");
}

TEST(ClassFinder, FindClassLocation) {
  EXPECT_NE(::open("Test.class", O_CREAT, 0644), -1);
  ClassLocation loc = findClassLocation("Test", {"."});
  EXPECT_EQ(loc.type, ClassLocation::File);
  EXPECT_EQ(loc.className, std::string("Test"));
  EXPECT_EQ(loc.path, std::string("./Test.class"));

  loc = findClassLocation("No exist", {"."});
  EXPECT_EQ(loc.type, ClassLocation::NoExist);
  EXPECT_EQ(loc.className, std::string());
  EXPECT_EQ(loc.path, std::string());
  (void)::remove("Test.class");
}

TEST(ClassFinder, FindFromJar) {
  EXPECT_NE(::open("Test.class", O_CREAT, 0644), -1);
  ASSERT_EQ(std::system("zip Test.jar Test.class > /dev/null"), 0);
  (void)::remove("Test.class");
  ClassLocation loc = findClassLocation("Test", {".", "Test.jar"});
  EXPECT_EQ(loc.type, ClassLocation::InJar);
  EXPECT_EQ(loc.className, std::string("Test"));
  EXPECT_EQ(loc.path, std::string("Test.jar"));
  (void)::remove("Test.jar");
}

TEST(ClassFinder, FindJavaLangObject) {
  const char *jhome = std::getenv("JAVA_HOME");
  ASSERT_TRUE(jhome) << "JAVA_HOME must be set";
  std::string path;
  std::string err = findRTJar(path);
  ASSERT_TRUE(err.empty()) << err;
  ClassLocation loc = findClassLocation("java/lang/Object", {path});
  EXPECT_EQ(loc.type, ClassLocation::InJar);
  EXPECT_EQ(loc.className, std::string("java/lang/Object"));
  EXPECT_EQ(loc.path, path);
}

TEST(ClassFinder, ZipFileBuffer) {
  std::ofstream s("file");
  s << "Hello";
  s.put(0);
  s.close();
  std::unique_ptr<FileBuffer> buff;

  if (std::system("zip Test.zip file > /dev/null"))
    goto cleanup;

  buff = ZipFileBuffer::create("Test.zip", "file");
  ASSERT_TRUE(buff);

  ASSERT_STREQ((const char *)*buff, "Hello");

cleanup:
  (void)::remove("file");
}
