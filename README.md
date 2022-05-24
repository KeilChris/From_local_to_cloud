# From local development to the cloud

This project is the example code shown in the webinar "From local to cloud". It is based on the
[AVH basic example](https://github.com/ARM-software/AVH-GetStarted/tree/main/basic), ported to different hardware (virtual and real). It demonstrates how to setup a development workflow with cloud-based Continuous Integration (CI) for testing an embedded application.

The embedded program implements a set of simple unit tests for execution on
a Arm Virtual Hardware (AVH) targets. Code development and debug can be done
locally, for example with [CMSIS-Build](https://arm-software.github.io/CMSIS_5/develop/Build/html/index.html) and [Keil MDK](https://developer.arm.com/tools-and-software/embedded/keil-mdk) tools.

Automated test execution is managed with GitHub Actions and gets triggered on
every code change in the repository. The program gets built and run on [Arm
Virtual Hardware](https://www.arm.com/products/development-tools/simulation/virtual-hardware) cloud infrastructure in AWS and the test results can
be then observed in repository's [GitHub Actions](https://github.com/KeilChris/From_local_to_cloud/actions).

## Further reading

- [Blog about cloud infrastructure for continuous integration tests](https://community.arm.com/arm-community-blogs/b/tools-software-ides-blog/posts/infrastructure-for-continuous-integration-tests)
- [Arm Virtual Hardware lab series](https://www.arm.com/campaigns/virtual-hardware-lab-series)
- [Register for your chance to earn free AWS EC2 CPU credits](https://www.arm.com/company/contact-us/virtual-hardware)
- [Arm Virtual Hardware documentation]([https://av](https://arm-software.github.io/AVH/main/overview/html/index.html))

## Example Structure

Folder or Files in the example       | Description
:------------------------------------|:--------------------
`./`                                 | Folder with the Basic embedded application example
`./RTE/Device/CMSDK_CM7_VHT/`        | Folder with target-specific configurable files for the AVH. Includes system startup files, linker scatter file, CMSIS-Driver configurations and others. See [Components in Project](https://www.keil.com/support/man/docs/uv4/uv4_ca_compinproj.htm) in µVision documentation.
`./RTE/Device/MIMXRT1052DVL6B/`      | Folder with target-specific configurable files for the i.MXRT1050 device.
`./main.c`                           | Application code.
`./CMSDK_CM7_VHT/retarget_stdio.c`   | AVH user retarget file for UART communication.
`./MIMXRT1050-EVKB/retarget_stdio.c` | i.MXRT1050 User retarget file for UART communication.
`./Basic.uvprojx`  <br/> `./Basic.uvoptx`  <br/> `./Basic.uvguix`  <br/>        | MDK project files
`./Basic.CMSDK_CM7_VHT.cprj`         | Project file for the AVH in [.cprj format](https://arm-software.github.io/CMSIS_5/Build/html/cprjFormat_pg.html)
`./vht_config.txt`                   | Configuration file for running the AVH model from command line.
`./mdk_config.txt`                   | Configuration file for running the AVH model model from within MDK.
`./build.py`                         | Python script for project build, execution and analysis of test results.
`./requirements.txt`                 | File with the list of Python packages required for execution of `./build.py`.
`./avh.yml`                          | File with instructions for [AVH Client for Python](https://github.com/ARM-software/avhclient).

## Prerequisites

The sections below list the installation and configuration requirements for
both supported use cases:

- execute the tests manually on a local machine
- run tests automatically in the AWS cloud

### Local environment setup

For building, running and debugging on the local machine one needs to install
the following tools.

#### Embedded Toolchain

- IDE for local build and debug (Windows only):
  - [Keil MDK](https://developer.arm.com/tools-and-software/embedded/keil-mdk), MDK-Professional Edition
- alternatively, for command-line build without debug (Linux, Windows):
  - [Arm Compiler 6 for Embedded](https://developer.arm.com/tools-and-software/embedded/arm-compiler)
    (also available with [Keil MDK](https://developer.arm.com/tools-and-software/embedded/keil-mdk)
    (Windows) or [Arm DS](https://developer.arm.com/tools-and-software/embedded/arm-development-studio)
    (Linux, Windows))
  - [CMSIS-Build](https://github.com/ARM-software/CMSIS_5/releases/download/5.8.0/cbuild_install.0.10.3.sh)
    command-line building tools provided with the [CMSIS_5 release](https://github.com/ARM-software/CMSIS_5/releases).
    Additionally requires for its operation:
    - [Bash](https://en.wikipedia.org/wiki/Bash_(Unix_shell)) environment.
    - [CMake](https://arm-software.github.io/CMSIS_5/Build/html/cmake.html)
      3.15 or above, and support for its build system (default is Ninja).
  - [Python 3.9](https://www.python.org/downloads/) (*optional*, needed only when using `build.py`)
    - with packages defined in `./requirements.txt`, that shall be installed with:\
      `pip install -r requirements.txt`

#### Targets

The MDK example project (Basic.uvprojx) contains two targets:

- Arm Virtual Hardware (AVH) model of an Arm Cortex-M7 core (based on the MPS2 development platform).
- NXP i.MXRT1052DVL6B Arm Cortex-M7 based device mounted on a MIMXRT1050-EVKB development board.

Note that CMSIS software packs used in the project will be requested and
installed automatically when using Keil MDK or CMSIS-Build.

### Cloud environment setup

Following setup is required for building and running the example program in the cloud as part of a
CI workflow.

- **Amazon Web Service (AWS) account** with:
  - Amazon EC2 (elastic cloud) access
  - Amazon S3 (storage) access
  - Registration to access AVH Amazon Machine Image [AVH AMI](https://aws.amazon.com/marketplace/search/results?searchTerms=Arm+Virtual+Hardware)
  - User role setup for scripted API access
- **GitHub**:
  - Fork of this repository with at least _Write_ access rights
  - Following AWS configuration values stored as
    [GitHub Secrets](https://docs.github.com/en/actions/security-guides/encrypted-secrets)
    of the forked repository
      Secret Name                                    | Description
      :----------------------------------------------|:--------------------
      `AWS_ACCESS_KEY_ID`<br>`AWS_SECRET_ACCESS_KEY` | [Access key pair](https://docs.aws.amazon.com/IAM/latest/UserGuide/id_credentials_access-keys.html) for the AWS account (as IAM user) that shall be used by the CI workflow for AWS access.
      `AWS_IAM_PROFILE`                              | The [IAM Role](https://docs.aws.amazon.com/IAM/latest/UserGuide/id_roles_use.html) to be used for AWS access.
      `AWS_S3_BUCKET_NAME`                           | The name of the S3 storage bucket to be used for data exchange between GitHub and AWS AMI.
      `AWS_DEFAULT_REGION`                           | The data center region the AVH AMI will be run on. For example `eu-west-1`.
      `AWS_EC2_SECURITY_GROUP_ID`                    | The id of the [VPC security group](https://docs.aws.amazon.com/vpc/latest/userguide/VPC_SecurityGroups.html) to add the EC2 instance to. Shall have format `sg-xxxxxxxx`.
      `AWS_SUBNET_ID`                                | The id of the [VPC subnet](https://docs.aws.amazon.com/vpc/latest/userguide/working-with-vpcs.html#view-subnet) to connect the EC2 instance to. Shall have format `subnet-xxxxxxxx`.

## Local build and debug

For developing the tests on the local machine one needs to clone this
repository into a local workspace.

### Build and debug in MDK

[Run with MDK-Professional](https://arm-software.github.io/AVH/main/infrastructure/html/run_mdk_pro.html)
explains in details the tool setup and project configuration for running an
MDK project on Arm Virtual Hardware.

For this example, open the `Basic.uvprojx` in MDK. Before launching the debug session of the AVH target,
one needs to verify the debugger configuration:

- Bring up the _Options for target..._ dialog from the tool bar.
- Navigate to the _Debug_ pane and select _Use: Models Cortex-M Debugger_.
- Next, click on the _Settings_ button to bring up the _Models Cortex-M Target Driver Setup_ dialog.
- Select in the as the _Command_ field the model executable for Cortex-M7 (filename is: `VHT_MPS2_Cortex-M7.exe`
  in the location where Virtual Hardware models are installed).
- Set `cpu0` as the _Target_.
- Browse for the _Configuration File_ and select `mdk_config.txt`.

Now, start the debug session and the model executable should pop up. By default,
MDK stops execution when reaching `main`. Set a breakpoint to line 37 and
continue execution. Hitting the breakpoint one can single step the code under
test to figure out the issue. In this case the issue is obvious:
`1 + (-1) != 2`.

There is another target in the project: MIMXRT1050-EVKB. This runs with the NXP MIMXRT1050-EVKB development
board. It shows the output of the unit test on a serial console. Use the on-board CMSIS-DAP debug adapter which
add a COM port to your PC to review the results.

### Building on command line

Open a command prompt in the local workspace. The following instructions assume
Python is installed. If one doesn't want to go the Python way one can issue the
individual command, manually. The CMSIS-Build command `cbuild` and the Virtual
Hardware Target executable `VHT_MPS2_Cortex-M7` are expected in the
system `PATH`.

In order to use the `build.py` script to conveniently execute the required commands,
one need to install some Python requirements listed in `requirements.txt`:

```text
$ pip install -f requirements.txt
```

Once the required Python packages are installed one can run the script to
build the example:

```text
$ ./build.py -t debug build
[debug](build:run_cbuild) bash -c cbuild.sh --quiet Basic.CMSDK_CM7_VHT.cprj
[debug](build:run_cbuild) bash succeeded with exit code 0

Matrix Summary
==============

target    build    run
--------  -------  -----
debug     success  (skip)

$ ls -lah Objects/basic.axf

-rw-r--r-- 1 **** 4096 64K Nov 25 10:59 Objects/basic.axf
```

The `build.py` script automatically creates a timestamped archive of the build results
in a file `basic-<timestamp>.zip`. One can extract this into another workspace at a
later point in time to inspect the result, e.g. run the tests in a debugger.

### Execute on command line

Open a command prompt in the local workspace. The following instructions assume
Python is installed. If one don't want to go the Python way one can issue the
individual command, manually.

```text
$ ./build.py -t debug run
[debug](run:run_vht) VHT_MPS2_Cortex-M7.EXE -q --stat --simlimit 10 -f vht_config.txt Objects/basic.axf
[debug](run:run_vht) VHT_MPS2_Cortex-M7.EXE succeeded with exit code 0
::set-output name=badge::Unittest-3%20of%204%20passed-yellow

Matrix Summary
==============

target    build    run
--------  -------  -----
debug     (skip)   3/4
```

The summary lists out the number of test cases passed and totally executed.
This example intentionally has one failing test case. Inspect the xunit result
file to see the details:

```text
$ cat basic-<timestamp>.xunit
<?xml version="1.0" ?>
<testsuites disabled="0" errors="0" failures="1" tests="4" time="0.0">
        <testsuite disabled="0" errors="0" failures="1" name="Cloud-CI basic tests" skipped="0" tests="4" time="0">
                <testcase name="test_my_sum_pos" file="main.c" line="57"/>
                <testcase name="test_my_sum_neg" file="main.c" line="58"/>
                <testcase name="test_my_sum_fail" file="main.c" line="48">
                        <failure type="failure" message="Expected 2 Was 0"/>
                </testcase>
                <testcase name="test_my_sum_zero" file="main.c" line="60"/>
        </testsuite>
</testsuites>
```

This reveals that the test assertion in `main.c` line 48 failed.

### Understanding the `build.py` script

The `build.py` script is written using the `python-matrix-runner` Python
package. Find more detailed documentation for this Python package on its
[PyPI page](https://pypi.org/project/python-matrix-runner/).

This section puts some parts of the build script into the spotlight.

#### Test report converter `class UnityReport`

The `class UnityReport` is implemented as a `ReportFilter` so that it can
later be used in a test report chain (see section about commands below).

The relevant part here is `stream` property that receives a data stream
(standard output of the model) and translates it into JUnit format. This
is done by pattern matching (using regular expression) on the expected
Unity framework output. Each found test case with its result is converted
into a `junit_xml.TestCase` taken from the Python package `junit-xml`.

#### Configuration axes `@matrix_axis`

The `python-matrix-runner` has built-in support for matrix build configurations.
In case of this trivial GetStarted example only a single configuration `debug`
is used. This is done by specifying a project specific `Enum` per degree of
freedom listing the configuration values. This example only has one axis
(`target`) with one value (`debug`). More elaborated use-cases may add further
axes for support of different compilers, optimization levels or target devices.

#### Build actions `@matrix_action`

The build script can offer different top-level actions the user can trigger
from the command line. In this example these are only `build` and `run`. Each
action is defined by the according method.

An action method must at least `yield` one shell command to be executed.
Complex actions can be composed from multiple shell commands interleaved
with additional Python code. In this basic example additional Python code
is used for some post-processing of the shell command results such as 
creating an zip archive with the build output.

#### Build commands `@matrix_command`

Each shell command (required to compose the actions) is defined by a method
returning an array with the command line parts, see
[subprocess.run](https://docs.python.org/3.8/library/subprocess.html#subprocess.run)
for details.

A command can be further customized, for instance by attaching a `test_report`.
The `test_report` is created by applying a chain of `ReportFiler`'s to the
output of the command. In the basic example the output of the Virtual Hardware
Target (VHT) model is captured (`ConsoleReport`), the Unity output between
the known text markers is cropped (`CropReport`), and the remaining data
is converted into JUnit format (`UnityReport`).

## Running tests in GitHub Actions CI

The repository defines a workflow to build and run the tests using
GitHub Actions on every change i.e., *push* and *pull_request* triggers.

To make this work the repository needs to be configured, see Prerequisites
above.

On every change, the workflow is kicked off executing the following steps.

- Execute build and test inside an EC2 instance\
  The [AVH Client for Python](https://github.com/ARM-software/avhclient) is used to
  - create new EC2 instance
  - upload the workspace to the EC2 instance using a S3 storage bucket;
  - run the command line build;
  - execute the test image using the AVH model
  - download the output into the workspace.
  - terminate the EC2 instance 
- Extract and post-process test output, including
  - conversion of the log file into XUnit format.
- Archive build/test output\
  The image, log file and XUnit report are attached as a build artifact for
  later analysis.
- Publish test results
