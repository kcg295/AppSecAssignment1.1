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
be installed by your local package manager or at
https://git-scm.com/downloads. For a cheat-sheet of git commands, please
see https://github.com/nyutandononline/CLI-Cheat-Sheet/blob/master/git-commands.md.
Although we will not be checking your commit messages or git log, it is
recommended that you write descriptive commit messages so that the
evolution of your repository is easy to understand. For a guide to
writing good commit messages, please read
https://chris.beams.io/posts/git-commit/ and
https://git.kernel.org/pub/scm/git/git.git/tree/Documentation/SubmittingPatches#n104
(lines 104 – 160).

After git is installed, you will want to configure your git user to sign
your commits. This can be done by following the guide at
https://git-scm.com/book/en/v2/Git-Tools-Signing-Your-Work.

In order for us to be able to verify that you successfully signed your commits,
please upload an ASCII armored copy of your signing key in the root directory
of your repository and name it signing\_key.pub.

After installing and configuring git, you should then create a GitHub
account at https://github.com, and add an ssh key using the guide found
at https://help.github.com/en/articles/adding-a-new-ssh-key-to-your-github-account.

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
.github/workflows directory in the root of your repository. After that, create a
workflow.yaml file in that directory, and set it up to work with GitHub Actions. For
information on how to set up GitHub Actions, you can read the GitHub Actions
tutorial (https://docs.github.com/en/actions/quickstart).
Finally, you are required to set up GitHub Actions. This will take the form
of configurations in the workflow YAML file mentioned earlier in the .github/workflows
directory. Information on how to configure GitHub Actions using this YAML file
can be found on the GitHub Actions Introduction
(https://docs.github.com/en/actions/learn-github-actions/introduction-to-github-actions)
In Part 2 of this project you will be expanding the functionality of GitHub
Actions by including tests.


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
its normal flow of control.

For this part, your job will be to find some flaws in the program, and
then create test cases (i.e., binary gift cards) that expose flaws in
the program. You should write:

1. *Two* test cases, `crash1.gft` and `crash2.gft`, that cause the
   program to crash; each crash should have a different root cause.
2. One test case, `hang.gft`, that causes the program to loop
   infinitely. (Hint: you may want to examine the "program" record type
   to find a bug that causes the program to loop infinitely.)
3. A markdown file, `writeup.md` with a heading called "bugs" explaining the bug
   triggered by each of your three test cases. 

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
GitHub Actions automatically build and run the program on your test suite.

## Part 3: Fuzzing and Coverage

As discussed in class, an important part of understanding how well your
test suite exercises your program's behaviors is *coverage*. To start
off, measure the coverage that your program achieves with the test cases
you created in Part 2. To do this, you should build `giftcardreader`
with the `--coverage` option to `gcc`, run your test suite, and then
produce a coverage report using `lcov` (details on how to do this can be
found in the lecture slides).

You should notice that there are portions of the program that are
*uncovered* (i.e., the code was not executed while processing your test
suite). Pick two lines of code from the program that are currently
not covered and create test cases that cover them.

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
tests, the fuzzer-generated tests, and a brief writeup explaining the
bugs you found and fixed in this part. This writeup should be in the writeup.md
file under a header named "testing".

## Grading

Total points: 100

Part 1 is worth 20 points:

* 10 points for signed commits
* 10 points for GitHub Actions configuration

Part 2 is worth 40 points:

* 10 points for your test cases and fixes
* 10 points for GitHub Actions regression testing
* 20 points for the bug writeup

Part 3 is worth 40 points:

* 10 points for handwritten tests
* 10 points for fuzzer-generated tests
* 10 points for your code fixes
* 10 points for writeup

## What to Submit

On NYU Classes, submit a link to your GitHub repository. The repository
should be **private**, and you should add the instructor/TA's GitHub
account as a contributor to give them access for grading.

For this section, your instructors are: Kevin Gallagher, GitHub ID `kcg295` and
Dean Christakos, GitHub ID to be added.

For this section, your TAs are Gaurav Chauhan, GitHub ID to be added, Sarthak
Bohra, GitHub ID to be added, and more TAs to be added.

The repository should contain:

* Part 1
  * Your GitHub Actions Workflow file
  * At least one signed commit
* Part 2
  * A directory named `tests` that contains `crash1.gft`, `crash2.gft`,
    `hang.gft`.
  * A markdown writeup called `writeup.md` with a heading named `bugs`
  * An updated GitHub Actions Workflow file that runs your tests
  * A commit with the fixed version of the code (if you like, this
    commit can also contain the files mentioned above)
* Part 3
  * A directory named `fuzz` that contains `cov1.gft`, `cov2.gft`,
    `fuzzer1.gft`, `fuzzer2.gft`.
  * `writeup.md` with a section named "testing".
  * An updated GitHub Actions Workflow file that runs the new tests
  * A commit with the fixed version of the code (if you like, this
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
