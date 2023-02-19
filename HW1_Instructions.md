# Homework 1: Beware of Geeks Bearing Gift Cards

## Introduction

You've been handed the unpleasant task of performing an audit of code
written by your company's least-favorite contractor, Shoddycorp's
Cut-Rate Contracting. The program is designed to read in gift card files
in a legacy binary file format, and then display them in either a simple
text output format, or in JSON format. Unfortunately, Shoddycorp isn't
returning your calls, so you'll have to make do with the comments in the
file and your own testing.

Justin Cappos (JAC) and Brendan Dolan-Gavitt (BDG) have read through the
code already. It's a mess. We've tried to annotate a few places that
look suspicious, but there's no doubt that there are many bugs lurking
in this code. Make no mistake, this is *not* an example of C code that
you want to imitate.

## Part 1: Setting up Your Environment

In order to complete this assignment, you are required to use the git
VCS. Before beginning to write your code, you should first install git
and set up a git repository to store your code in. The git binaries can
be installed by your local package manager or [here](https://git-scm.com/downloads). For a cheat-sheet of git commands, please
see [this](https://github.com/nyutandononline/CLI-Cheat-Sheet/blob/master/git-commands.md).
Although we will not be checking your commit messages or git log, it is
recommended that you write descriptive commit messages so that the
evolution of your repository is easy to understand. For a guide to
writing good commit messages, please read
[this](https://chris.beams.io/posts/git-commit/) and
[this](https://git.kernel.org/pub/scm/git/git.git/tree/Documentation/SubmittingPatches#n104)
at lines 104 - 160.

After git is installed, you will want to configure your git user to sign
your commits. This can be done by following the guide 
[here](https://docs.github.com/en/authentication/managing-commit-signature-verification/generating-a-new-gpg-key).

In order for us to be able to verify that you successfully signed your commits,
please upload an ASCII armored copy of your signing key in the root directory
of your repository and name it signing_key.pub.

After installing and configuring git, you should then create a [GitHub
account](https://github.com), and add an ssh key using this 
[guide](https://help.github.com/en/articles/adding-a-new-ssh-key-to-your-github-account).

After creating a GitHub account, you are ready to create a git
repository for the project you will be working on. The recommended way
to do this is to create a repository using the GitHub Web interface.
This is recommended because it can automatically generate important
files for you, such as the license for your project and the gitignore
file to avoid unintentionally tracking unnecessary files. After you
create the repository on GitHub, you can clone it to your local machine
using the following command.

```
$ git clone git@github.com:<your_username>/<your_repository_name>.git
```

Be sure to make the repository **private**.

The next step is to set up GitHub Actions CI. To do this,you should create a
`.github/workflows` directory in the root of your repository. After that, create a
`workflow.yaml` file in that directory, and set it up to work with GitHub Actions. For
information on how to set up GitHub Actions, you can read the [GitHub Actions
tutorial](https://docs.github.com/en/actions/quickstart).
Finally, you are required to set up GitHub Actions. This will take the form
of configurations in the workflow YAML file mentioned earlier in the .github/workflows
directory. Information on how to configure GitHub Actions using this YAML file
can be found on the 
[GitHub Actions Introduction](https://docs.github.com/en/actions/learn-github-actions/introduction-to-github-actions).
You can feel free to look [here](https://github.com/actions/starter-workflows) for template YAML files, in particular 
[this one for C/C++ projects](https://github.com/actions/starter-workflows/blob/main/ci/c-cpp.yml) to set up your YAML file.

In Part 2 and Part 3 of this project you will be expanding the functionality of GitHub
Actions by including new tests.


## Part 2: Auditing and Test Cases

Start off by copying the files from this repository into your own, and
add them to git. The files you need are:

```
examplefile.gft giftcardexamplewriter.c giftcard.h giftcardreader.c Makefile
```

Read through the `giftcardreader.c` program (and its accompanying
header, `giftcard.h`) to get a feel for it. You should also try building
and running it with the included `examplefile.gft` file, to see what its
normal output is. You may find it helpful to use a debugger like `gdb`
to step through the program as it executes, to get an understanding of
its normal flow of control. Another helpful tool is [Valgrind](https://valgrind.org/docs/manual/quick-start.html)
to help you find where segmentation faults occur and identifying its cause.

For this part, your job will be to find some flaws in the program, and
then create test cases (i.e., binary gift cards) that expose flaws in
the program. You should write:

1. *Two* test cases, `crash1.gft`, and `crash2.gft`, that cause the
   program to crash; each crash should have a different root cause. These root causes are:
      1. A crash that's caused by a null pointer in line 190 of giftcardreader.c
      2. A crash that occurs in the animate function (lines 16 - 67 of giftcardreader.c).
2. One test case, `hang.gft`, that causes the program to loop
   infinitely. (Hint: you may want to examine the "program" record type
   to find a bug that causes the program to loop infinitely.)
3. Find a third crash that is giftcard agnostic, that is, that does not
   require a specific gift card to perform. *Hint: check the main function.* You must also
   fix this bug.

To create your own test files, you may want to look at
`giftcardexamplewriter.c`. Although it is no better written than
`giftcardreader.c`, it should help you understand the basics of the file
format and how to produce gift card files.

Finally, fix the bugs that are triggered by your test cases, and verify
that the program no longer crashes / hangs on your test cases. Above each bug
that you fix, please put a comment that states what bug it fixes. For example,
if you fix crash1 on line 22, on line 21 you should put "//crash1". **This is
necessary to ensure that your code fixes are spotted by our autograder!** To
make sure that these bugs don't come up again as the code evolves, have
*GitHub Actions automatically build and run the program on your test suite*.

## Part 3: Fuzzing and Coverage

### Coverage
As discussed in class, an important part of understanding how well your
test suite exercises your program's behaviors is *coverage*. To start
off, measure the coverage that your program achieves with the test cases
you created in Part 2. To do this, you should build `giftcardreader`
with the `--coverage` option to `gcc`, run your test suite, and then
produce a coverage report using `lcov` (details on how to do this can be
found in the lecture slides).

You should notice that there are portions of the program that are
*uncovered* when you run the giftcardreader with the initial `examplefile.gft` 
(i.e., the code was not executed while processing your test
suite). Create two test cases that each cover new lines of code from the original case.
In the writeup, explain which new lines of code each new test case covered compared to the original.

### Fuzzing
**You can use the giftcardreader.c without fixes from Part 2**  
An easy and effective way of finding crashes and getting higher coverage
in a program is to *fuzz* it with a fuzzer like AFL. Fuzz the program
using AFL, following the [quick-start
instructions](https://lcamtuf.coredump.cx/afl/QuickStartGuide.txt). To
make the fuzzing more effective, you should provide AFL
with all of the test files you have created in its input directory. Let
the fuzzer run for at least two hours, and then examine the test cases
(in the `queue` directory) and crashes/hangs (in the `crashes` and
`hangs` directories).

Add the generated test cases (but not the crashes or hangs) to your test
suite, and produce a new coverage report. You should see that the tests
generated by the fuzzer reach more parts of the gift card program.

Finally, pick two crashes/hangs and fix the bugs in the program that
cause them. Then, re-run the program on *all* the crashes and hangs
found by AFL. You should find that not all of the crashes found by AFL
originally crash the program now---although AFL tries its best to figure
out which crashes are caused by unique bugs, if often overcounts.

Add the generated tests to your repository and have GitHub Actions run them.
Note that depending on how long you ran the fuzzer and how fast your
machine is, there may be a lot of redundant test cases! To keep only the
ones that exercise new behavior in your program, you can use the
`afl-tmin` tool.

To complete the assignment, commit your updated code, your handwritten 
tests to extend code coverage and the fuzzer-generated tests.

## Grading

Total points: 100

Part 1 is worth 20 points:

* 10 points for signed commits
* 10 points for GitHub Actions configuration

Part 2 is worth 40 points:

* 12 points for your test cases and fixes
* 08 points for GitHub Actions regression testing
* 20 points for the bug writeup (See HW1 - Written)

Part 3 is worth 40 points:

* 8 points for extending coverage using cov1.gft and cov2.gft and explanation (See HW1 - Written)
* 2 points for GitHub Actions regression testing with fuzzer-generated tests
* 10 points for fuzzer-generated tests
* 10 points for your code fixes for the fuzzer-generated tests
* 10 points for writeup on fuzzer-generated tests (See HW1 - Written)

## What to Submit
For this assignment, there are two submissions, `HW1 - Written` and `HW1 - Code Submission`.

### HW1 - Written
Note: you don't need to provide code submissions or screenshots, just a simple writeup showing you understand what you 
did is sufficient!
Please submit a pdf file to gradescope that contains the following writeup:

* Part 2
  * 2.5 points for an explanation of how `crash1.gft` caused `giftcardreader` to crash
  * 2.5 points for an explanation of you fixed `giftcardreader` to NOT crash with `crash1.gft`
  * 2.5 points for an explanation of how `crash2.gft` caused `giftcardreader` to crash
  * 2.5 points for an explanation of you fixed `giftcardreader` to NOT crash with `crash2.gft`
  * 2.5 points for an explanation of how `crash3.gft` caused `giftcardreader` to crash
  * 2.5 points for an explanation of you fixed `giftcardreader` to NOT crash with `crash3.gft`
  * 2.5 points for an explanation of how `hang.gft` caused `giftcardreader` to hang
  * 2.5 points for an explanation of you fixed `giftcardreader` to NOT hang with `hang.gft`
* Part 3
  * 2.5 points for an explanation of how `fuzzer1.gft` caused `giftcardreader` to crash
  * 2.5 points for an explanation of you fixed `giftcardreader` to NOT crash with `fuzzer1.gft`
  * 2.5 points for an explanation of how `fuzzer2.gft` caused `giftcardreader` to crash
  * 2.5 points for an explanation of you fixed `giftcardreader` to NOT crash with `fuzzer2.gft`
  * 2.0 points for an explanation of what NEW lines of code was covered with `cov1.gft`
  * 2.0 points for an explanation of what NEW lines of code was covered with `cov1.gft`
  
### HW1 - Code Submission
**Please submit your code/repository to the auto-grader on Gradescope!** The remaining parts of the code will be 
checked manually when you share your GitHub Repository (e. g. git signed commit).

Your GitHub repository should be **private**, and you should add the instructor/TA's GitHub
account as a contributor to give them access for grading. Please see your course page to see who your grader is. Please only add your
grader the day you turn your assignment in.

The repository should contain:

* Part 1
  * Your GitHub Actions Workflow file
  * At least one signed commit
* Part 2
  * A directory naed `tests` that contains `crash1.gft`, `crash2.gft`,
    `hang.gft`.
  * A writeup explaining your work completed in Part 2 to Gradescope
  * An updated GitHub Actions Workflow file that runs your tests
  * A commit with the fixed version of the code, named `giftcardreader.c` (if you like, this
    commit can also contain the files mentioned above)
* Part 3
  * A directory named `fuzz` that contains `cov1.gft`, `cov2.gft`,
    `fuzzer1.gft`, `fuzzer2.gft`.
  * A writeup explaining your work completed in Part 3 to Gradescope
  * An updated GitHub Actions Workflow file that runs the new tests
  * A commit with the fixed version of the code, named `giftcardreader.c` (if you like, this
    commit can also contain the files mentioned above)

## Concluding Remarks

Despite the fixes you've made, there are almost certainly still many
bugs lurking in the program. Although it is possible to get to a secure
program by repeatedly finding and fixing bugs (at least when the program
is this small), it's a lot of work, and just because a fuzzer stops
finding bugs doesn't mean that the program is bug-free!

Realistically, this program is probably unsalvageable in its current
state. It would be better in this case to rewrite it from scratch,
either in C using a very defensive programming style, or in a safer
language like Python or Rust. Once this assignment is complete, we will
release a cleanly-written version of the program (written in C) that
should be relatively bug-free [1]. You'll notice that it's a lot more
verbose, and checks for many more errors than the buggy
version---writing safe C code is difficult!

[1] Although you are encouraged to try to prove us wrong by finding bugs
    in it!
