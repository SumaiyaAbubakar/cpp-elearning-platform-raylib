# cpp-elearning-platform-raylib
<div align="center">

<img src="assets/readme-hero.svg" alt="SAM — E-Learning Platform. A role-based desktop learning platform written in C++ with raylib." width="100%">

<br>

![C++17](https://img.shields.io/badge/C%2B%2B-17-8250c8?style=flat-square&logo=cplusplus&logoColor=white)
![raylib 5.0](https://img.shields.io/badge/raylib-5.0-ff6496?style=flat-square)
![Desktop GUI](https://img.shields.io/badge/interface-desktop%20GUI-78d2b4?style=flat-square)
![Storage CSV](https://img.shields.io/badge/storage-CSV%20files-64c8ff?style=flat-square)
![OOP model](https://img.shields.io/badge/OOP%20model-24%20classes-ffc864?style=flat-square)

**[Screens](#-the-application) · [Features](#-what-it-does) · [Architecture](#-architecture) · [OOP showcase](#-oop-showcase) · [Run it](#-getting-started) · [Roadmap](#-roadmap)**

</div>

<img src="assets/divider.svg" alt="" width="100%">

## ✦ Overview

**SAM** (Smart Academic Manager) is a desktop learning platform where **students, teachers and admins** each get their own workspace. Students enroll in courses, take quizzes, track tasks and read their gradebook. Teachers publish courses, post assignments and grade submissions. Admins approve courses, manage accounts and watch enrollments and subscription plans.

It is written in **C++17**, drawn entirely with **raylib** (immediate-mode GUI, 23 screens, no GUI framework), and stores everything in plain **CSV files** through a small `FileHandler` class, so there is no database to install. Alongside the running app, the repository contains a **24-class object-oriented domain model** (abstract bases, interfaces, polymorphic courses, exceptions, operator overloading) described in detail [below](#-oop-showcase).

<img src="assets/divider.svg" alt="" width="100%">

## ✦ The application

Every image below is a real capture of the compiled app, not a mock-up. They are cropped to the used area of the 1920×1080 window.

<div align="center">
<img src="assets/screenshots/home.png" alt="SAM home screen with Auth, Teacher and Admin quick-access cards" width="100%">
<br><sub><b>Home</b> — quick access cards for the auth, teacher and admin areas</sub>
</div>

<br>

<table>
<tr>
<td width="50%" align="center"><img src="assets/screenshots/student-dashboard.png" alt="Student dashboard with enrolled courses, average grade, pending tasks and zoom link counters"><br><sub><b>Student dashboard</b> — live counters and course table</sub></td>
<td width="50%" align="center"><img src="assets/screenshots/teacher-dashboard.png" alt="Teacher dashboard with course, submission and zoom link counters"><br><sub><b>Teacher dashboard</b> — courses with Approved / Pending status</sub></td>
</tr>
<tr>
<td width="50%" align="center"><img src="assets/screenshots/admin-dashboard.png" alt="Admin dashboard with user, course, pending approval and enrollment counters and a users table"><br><sub><b>Admin dashboard</b> — users, pending approvals, enrollments</sub></td>
<td width="50%" align="center"><img src="assets/screenshots/admin-subscriptions.png" alt="Admin subscription plans: Basic, Pro and Premium, with an enrollment table"><br><sub><b>Subscriptions</b> — Basic, Pro and Premium plans</sub></td>
</tr>
<tr>
<td width="50%" align="center"><img src="assets/screenshots/quiz.png" alt="Quiz screen with three multiple-choice questions and answers selected"><br><sub><b>Quiz</b> — multiple choice with instant scoring</sub></td>
<td width="50%" align="center"><img src="assets/screenshots/admin-approve-courses.png" alt="Admin approve courses table with Approve and Reject buttons on a pending course"><br><sub><b>Approve courses</b> — one click to approve or reject</sub></td>
</tr>
</table>

<img src="assets/divider.svg" alt="" width="100%">

## ✦ What it does

<table>
<tr>
<td width="33%" valign="top">

### 🔐 Role-based login
Pick **Student, Teacher or Admin** at login. Credentials are checked by `FileHandler::checkLogin`, and a correct password with the wrong role is rejected. The three dashboards are guarded by `requireRole(...)`.

</td>
<td width="33%" valign="top">

### 🎓 Student workspace
Dashboard counters, a course catalogue with **enroll-by-ID**, a gradebook with **PASS / FAIL** at 50% and a class average, plus a personal **task planner** with High / Medium / Low priority.

</td>
<td width="33%" valign="top">

### 🏅 Quiz and badges
A built-in three-question OOP quiz scored out of 30 that awards a **Bronze to Platinum** tier and logs each attempt. The gradebook turns your average into a badge too: Bronze 40%, Silver 60%, Gold 75%, Platinum 90%.

</td>
</tr>
<tr>
<td valign="top">

### 🧑‍🏫 Teacher studio
Create **Free or Paid** courses (they enter as *Pending*), record uploaded materials, create assignments with deadline and max marks, **grade submissions by ID** and add gradebook entries.

</td>
<td valign="top">

### 🛡️ Admin console
**Approve or reject** courses, **activate, ban or remove** accounts, **reset passwords**, and review every enrollment with its *Paid* and *Confirmed* state.

</td>
<td valign="top">

### 💳 Subscription plans
Basic (Rs. 999/mo), Pro (Rs. 2999/mo) and Premium (Rs. 5999/mo) cards, with **activate / cancel** by student ID recorded as paid enrollments.

</td>
</tr>
<tr>
<td valign="top">

### 📹 Class links
Add **Zoom links** with teacher, section, course and scheduled time; everything lands in one table that students can open from their dashboard.

</td>
<td valign="top">

### 🗂️ Plain-file persistence
Ten CSV files under `data/`, written by `FileHandler` with a quote-aware CSV reader and writer. First run **seeds demo data** automatically.

</td>
<td valign="top">

### 🧾 Activity log
Registrations, enrollments, course changes, grading, password resets and quiz attempts are appended to `activity_log.csv`.

</td>
</tr>
</table>

### How a course moves through the system

```mermaid
%%{init: {'theme':'dark','themeVariables':{'primaryColor':'#3b2a66','primaryBorderColor':'#8250c8','lineColor':'#ff6496','secondaryColor':'#1f4a40','tertiaryColor':'#1c3a55','fontSize':'14px'}}}%%
flowchart LR
    subgraph T["🧑‍🏫 Teacher"]
        A["Add course<br/>(Free / Paid)"]
        G["Grade submission<br/>by ID"]
    end
    subgraph AD["🛡️ Admin"]
        B{"Approve<br/>or Reject"}
    end
    subgraph S["🎓 Student"]
        C["Enroll<br/>by course ID"]
        D["Take quiz"]
        E["Gradebook<br/>+ badge tier"]
    end
    A -->|"status = Pending"| B
    B -->|"Approved / Rejected"| C
    C --> D --> E
    G -->|"grade row saved"| E
    A -.->|courses.csv| F[("data/*.csv")]
    C -.->|enrollments.csv| F
    G -.->|grades.csv| F
```

<img src="assets/divider.svg" alt="" width="100%">

## ✦ Architecture

<div align="center">
<img src="assets/architecture.svg" alt="Architecture: gui.cpp calls FileHandler which reads and writes CSV files in data/. A separate 24-class OOP domain model lives in main (1).cpp." width="100%">
</div>

<br>

| Layer | Files | Responsibility |
|---|---|---|
| **GUI** | `gui.cpp`, `gui.h` | `main()` loop, the `Screen` enum (23 IDs), one `DrawXxx()` function per screen, shared widgets (`DrawButton`, `DrawInputBox`, `DrawPanel`), role guard, theme colors |
| **Persistence** | `fileHandlerdemo.h`, `fileHandlerdemo (1).cpp` | `FileHandler` class, nine record structs, `CSV` namespace helpers (`nextField`, `escField`, `copyStr`), activity log |
| **Domain model** | `main (1).cpp` | The 24 OOP classes: users, courses, quizzes, grading, approvals, subscriptions. A standalone design with no `main()` of its own |

The GUI keeps in-memory arrays (`gUsers`, `gCourses`, `gGrades`, …) filled by `reloadAll()`, draws them every frame, and writes them back through `FileHandler` whenever a button changes something.

<img src="assets/divider.svg" alt="" width="100%">

## ✦ OOP showcase

The class model in `main (1).cpp` captures the whole platform as objects. Two diagrams below, drawn from the real declarations: the **people and course core**, then the **learning, grading and governance** classes around it.

### Core: users, courses and student helpers

```mermaid
%%{init: {'theme':'dark','themeVariables':{'primaryColor':'#3b2a66','primaryBorderColor':'#8250c8','lineColor':'#ff6496','fontSize':'13px'}}}%%
classDiagram
    direction TB
    class User {
        <<abstract>>
        #string userID
        #string name
        #string email
        #string password
        #string role
        #bool isActive
        #int totalUsers$
        +login(email, password)* bool
        +viewProfile()* void
        +changePassword(newPass) void
        +getTotalUsers()$ int
    }
    class Student {
        -Course* enrolledCourses[50]
        -float overallPercent
        -Badge badge
        -ToDoList todoList
        -ZoomLink zoomLinks[20]
        +enroll(Course*) void
        +attemptQuiz(Quiz*) void
        +submitAssignment(Assignment*) void
        +earnBadge() void
    }
    class Teacher {
        -Course* courses[50]
        +uploadMaterial(Course*, path) void
        +gradeSubmission(Submission*, mark) void
        +submitCourseForApproval(Course*) void
    }
    class Admin {
        -Course* managedCourses[100]
        +approveCourse(id) void
        +removeUser(id) void
        +manageSubscriptions() void
    }
    class Course {
        <<abstract>>
        #string courseID
        #float price
        #bool isApproved
        #CourseMaterial* materials[50]
        #Quiz* quizzes[50]
        #Assignment* assignments[50]
        +displayInfo()* void
        +enroll(Student*)* void
        +approve() void
    }
    class FreeCourse {
        -string accessLevel
        +displayInfo() void
        +enroll(Student*) void
    }
    class PaidCourse {
        -string subscriptionPlan
        -bool paymentRequired
        +displayInfo() void
        +enroll(Student*) void
        +processPayment(Student*) bool
    }
    class Badge {
        -string name
        -float minPercentage
        +award(float percentage) void
    }
    class ToDoList {
        -TASK tasks[100]
        +addTasks(TASK) bool
        +sortbyPriority() void
        +viewPending() void
    }
    class TASK {
        -string priority
        -bool isDone
        +markDone() void
        +priorityRank() int
    }
    class ZoomLink {
        -string url
        -string scheduledTime
        +openlink() void
        +operator_greater_than(string) bool
    }
    User <|-- Student
    User <|-- Teacher
    User <|-- Admin
    Course <|-- FreeCourse
    Course <|-- PaidCourse
    Student *-- Badge : owns
    Student *-- ToDoList : owns
    Student *-- ZoomLink : owns 20
    ToDoList *-- TASK : holds 100
    Student o-- Course : enrolled in
    Teacher o-- Course : teaches
    Admin o-- Course : manages
```

### Around the core: learning, grading and governance

```mermaid
%%{init: {'theme':'dark','themeVariables':{'primaryColor':'#1f4a40','primaryBorderColor':'#78d2b4','lineColor':'#ffdc64','fontSize':'13px'}}}%%
classDiagram
    direction LR
    class IApprovable {
        <<interface>>
        #approve()* void
        #reject()* void
        #getStatus()* string
    }
    class RegistrationApproval {
        -int requestID
        -string userRole
        -string status
        -validateRole(role) string
        +approve() void
        +reject() void
        +getStatus() string
    }
    class CourseManager {
        -Course* pendingCourses[100]
        -Course* approvedCourses[100]
        -Course* rejectedCourses[100]
        +submitCourse(Course*) void
        +approveCourse(id) void
        +rejectCourses(id) void
    }
    class AccountManager {
        -User* userlist[100]
        +addUser(User*, role) void
        +removeStudent(id) void
        +resetPassword(id, pass) void
        +printAll() void
    }
    class Subscription {
        -string planName
        -float price
        -Subscription* catalogue[10]$
        +activate(studentID) void
        +cancel(studentID) void
        +listPlans()$ void
    }
    class Quiz {
        -Question questions[50]
        -int durationMinutes
        +addQuestion(Question) void
        +autoGrade(answers[], n) float
        +attempt() float
    }
    class Question {
        -string options[4]
        -int correctAnswerIndex
        +checkAnswer(index) bool
    }
    class Assignment {
        -string deadline
        -int maxMarks
    }
    class Submission {
        -float grade
        -bool isGraded
        +markGraded(mark) void
    }
    class CourseMaterial {
        -string fileType
        -string filePath
        +download() void
    }
    class GradeBook {
        -float studentGrades[100]
        -float classAverage
        -recalculateAverage() void
        +addGrade(studentID, mark) void
    }
    class Enrollment {
        -bool paymentStatus
        -bool isConfirmed
        +confirm() void
        +cancel() void
    }
    class PrerequisiteChecker {
        -int completedCourseIDs[100]
        +check(studentID, prereqID) bool
    }
    IApprovable <|-- RegistrationApproval
    Quiz *-- Question : holds 50
    Course o-- CourseMaterial
    Course o-- Quiz
    Course o-- Assignment
    CourseManager o-- Course : pending / approved / rejected
    AccountManager o-- User : lists
    Teacher ..> Submission : gradeSubmission
    Student ..> Quiz : attemptQuiz
    PaidCourse ..> Student : processPayment
```

<sub>`*` abstract / pure virtual · `$` static · `#` protected, `-` private, `+` public. `ZoomLink::operator_greater_than` is the diagram's spelling of `operator>`, which Mermaid cannot display. `GradeBook`, `Enrollment`, `PrerequisiteChecker`, `Assignment` and `Submission` are self-contained building blocks that other classes reference by ID or pointer.</sub>

### OOP concepts, mapped to real code

| Concept | Where it lives | What it does |
|---|---|---|
| **Abstraction** | `User` (L20), `Course` (L292), `IApprovable` (L1567) | Pure virtual `login`, `viewProfile`, `displayInfo`, `enroll`, `approve`, `reject`, `getStatus` define contracts the base classes cannot fulfil themselves |
| **Inheritance** | `Student`, `Teacher`, `Admin` ← `User`; `FreeCourse`, `PaidCourse` ← `Course`; `RegistrationApproval` ← `IApprovable` | Derived constructors chain upward, e.g. `Teacher : User("", name, email, password, "Teacher")` |
| **Polymorphism** | `Course::enroll`, `Course::displayInfo`, `User::viewProfile` | `FreeCourse::enroll` enrolls straight away; `PaidCourse::enroll` checks approval, then calls `processPayment`. `CourseManager::getPending` and `AccountManager::printAll` call the overridden methods through base-class pointers |
| **Encapsulation** | every class | Data is `private` or `protected`; behaviour is exposed through getters and methods. Internal helpers stay private: `GradeBook::recalculateAverage`, `CourseManager::findInAll`, `RegistrationApproval::validateRole`, `FileHandler::buildPath` |
| **Composition** | `Student`, `ToDoList`, `Quiz` | `Student` owns a `Badge`, a `ToDoList` and `ZoomLink[20]` by value; `ToDoList` owns `TASK[100]`; `Quiz` owns `Question[50]` |
| **Aggregation** | `Course`, `CourseManager`, `AccountManager`, `Teacher`, `Student`, `Admin` | Hold `Course*` / `User*` / `Quiz*` pointers to objects they do not own |
| **Static members** | `User::totalUsers`, per-class `nextID`, `Subscription::catalogue` | Auto-incrementing IDs in `Question`, `Quiz`, `Assignment`, `Submission`, `Enrollment`, `CourseMaterial`, `Badge`, `TASK`; a self-registering plan catalogue with `Subscription::listPlans()` |
| **Operator overloading** | `ZoomLink::operator>(const string&)` (L1368) | Compares a link's `scheduledTime` with a given time string |
| **Exception handling** | `RegistrationApproval`, `CourseManager`, `AccountManager`, `Subscription` | 29 `throw` sites using `invalid_argument`, `runtime_error`, `out_of_range`, `overflow_error`, for example approving a request twice or submitting a duplicate course |
| **Constructors** | `Badge`, `TASK`, `ToDoList`, `Question` | Default and parameterised constructors, member-initialiser lists, default arguments such as `prereqID = 0` |
| **const-correctness** | `display() const`, getters, `addTasks(const TASK&)` | Read-only methods are marked `const` |
| **Namespaces** | `namespace CSV` | CSV parsing helpers kept out of the global scope |

### Objects in action

This snippet was compiled and run against the real `main (1).cpp`:

```cpp
#include "main (1).cpp"

int main() {
    Student       ali("Ali Khan", "ali@edu.pk", "pass123");
    FreeCourse    intro("C001", "Intro to C++", "Learn C++ from scratch", 0, 1);
    PaidCourse    ds("C002", "Data Structures", "Advanced DS with C++", 4999, 1, "Pro");

    CourseManager cm;
    cm.submitCourse(&intro);
    cm.submitCourse(&ds);
    cm.approveCourse("C002");
    ds.approve();

    Course* catalog[] = { &intro, &ds };      // one array, two behaviours
    for (Course* c : catalog) c->enroll(&ali);

    try {
        RegistrationApproval r(5, "Student");
        r.approve();
        r.approve();                           // second approval throws
    } catch (const runtime_error& e) {
        cout << "caught: " << e.what() << endl;
    }
}
```

```text
Enrolling in free course: Intro to C++
Ali Khan enrolled in: Intro to C++
Enrolled successfully!
Attempting to enroll in paid course: Data Structures
Processing payment of Rs.4999...
Payment successful!
Ali Khan enrolled in: Data Structures
Enrolled in paid course successfully!
caught: Request 1 already processed
```

### Design decisions

- **Contracts first.** `User` and `Course` declare *what* every subclass must do and leave *how* to the subclass, so new user types or course types plug in without touching the callers.
- **Managers hold pointers, not copies.** `CourseManager` moves a `Course*` between pending, approved and rejected lists; the course object itself never moves.
- **Fail loudly in the model.** Invalid roles, duplicate IDs, full lists and double approvals throw typed exceptions instead of silently doing nothing.
- **Presentation and storage stay apart.** The GUI never touches files directly; all disk access goes through `FileHandler`.

<img src="assets/divider.svg" alt="" width="100%">

## ✦ Data files

`FileHandler` reads and writes these files in `data/` (created on first run). Each `load*` / `save*` pair maps a CSV file to a record struct.

| File | Record struct | Used by |
|---|---|---|
| `users.csv` | `UserRecord` | login, registration, admin user tools |
| `courses.csv` | `CourseRecord` | catalogue, teacher courses, approvals |
| `enrollments.csv` | `EnrollmentRecord` | enrolling, subscriptions, monitoring |
| `grades.csv` | `GradeRecord` | gradebooks |
| `tasks.csv` | `TaskRecord` | student task planner |
| `zoom.csv` | `ZoomLinkRecord` | class links |
| `submissions.csv` | `SubmissionRecord` | grading |
| `assignments.csv` | `AssignmentRecord` | assignments and uploaded-material records |
| `materials.csv` | `MaterialRecord` | supported by `FileHandler`; not yet used by a screen |
| `activity_log.csv` | — | append-only audit trail |

## ✦ Tech stack

| | |
|---|---|
| **Language** | C++17 |
| **GUI / rendering** | [raylib 5.0](https://www.raylib.com/) (immediate-mode drawing and input) |
| **Standard library** | `<string>`, `<fstream>`, `<iostream>`, `<stdexcept>`, `<cstring>` |
| **Storage** | CSV files via `FileHandler` |
| **Compiler** | `g++` (GCC 13.3 used for verification) |

## ✦ Getting started

### Prerequisites

- A C++17 compiler (`g++`)
- **raylib 5.0** headers and library for your platform
- A desktop session (the app opens a window)

### 1 · Clone

```bash
git clone <your-repository-url>
cd <your-repository-folder>
```

### 2 · Build and run on Linux (verified on Ubuntu 24.04)

```bash
# toolchain and raylib's X11/OpenGL dependencies
sudo apt install build-essential git libgl-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev

# raylib 5.0 as a static library
git clone --depth 1 --branch 5.0 https://github.com/raysan5/raylib.git
make -C raylib/src PLATFORM=PLATFORM_DESKTOP

# compile BOTH source files
g++ -std=c++17 gui.cpp "fileHandlerdemo (1).cpp" -o SAM \
    -I raylib/src -L raylib/src -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

./SAM
```

### Build on Windows (MinGW-w64)

*Not verified in the environment used to write this README.* The repository's `libraylib.a` is a Windows/MinGW archive, so with raylib 5.0 installed (for example in `C:/raylib`):

```bash
g++ -std=c++17 gui.cpp "fileHandlerdemo (1).cpp" -o SAM -I"C:/raylib/include" -L"C:/raylib/lib" -lraylib -lopengl32 -lgdi32 -lwinmm
./SAM
```

### Demo accounts

On first launch, if `data/users.csv` is empty, SAM seeds three accounts and sample data:

| Role | Email | Password |
|---|---|---|
| Student | `ali@edu.pk` | `pass123` |
| Teacher | `sana@edu.pk` | `teach99` |
| Admin | `admin@edu.pk` | `admin123` |

Choose the matching role button on the login screen. Delete the `data/` folder to reset everything.

### Try the OOP model on its own

`main (1).cpp` has no `main()`, so write a small driver (like the one in [Objects in action](#objects-in-action)) and compile it by itself:

```bash
g++ -std=c++17 demo.cpp -o demo && ./demo
```

### Troubleshooting

| Symptom | Fix |
|---|---|
| `undefined reference to FileHandler::…` | Compile **both** `gui.cpp` and `fileHandlerdemo (1).cpp`, as in the commands above |
| `raylib.h: No such file or directory` | Pass `-I` pointing at your raylib 5.0 `src` (or `include`) folder |
| Linker errors for `X11`, `GL`, `Xrandr` | Install the `-dev` packages listed above |
| `data/` appears in the wrong place | Run `./SAM` from the repository root; the folder is created in the working directory |
| Window will not open over SSH or in a container | raylib needs a display; run on a desktop session |

## ✦ Project structure

```text
.
├── gui.cpp                      # main(), all 23 screens, widgets, role guard
├── gui.h                        # Screen enum, theme colors, shared declarations
├── fileHandlerdemo.h            # record structs, CSV namespace, FileHandler class
├── fileHandlerdemo (1).cpp      # FileHandler + CSV helper implementations
├── main (1).cpp                 # 24-class OOP domain model (no main())
├── ClassDiagram_ELearning.docx  # original class diagram document
├── ELearning_OOP_Project_FinalReport.docx
├── assets/
│   ├── readme-hero.svg          # animated banner
│   ├── architecture.svg         # layer diagram
│   ├── divider.svg
│   └── screenshots/             # real captures of the running app
└── data/                        # CSV storage, created on first run
```

## ✦ Roadmap

The running app and the class model were built side by side. Natural next steps:

- **Connect the two halves.** Have the GUI create and use `Student`, `Course`, `Quiz` and friends instead of passing plain record structs to `FileHandler`.
- **Guard every screen.** Today the three dashboards call `requireRole`; the teacher and admin screens reachable from the Home cards do not.
- **Secure credentials.** Store salted password hashes instead of plain text in `users.csv`.
- **Enforce account approval.** Registration currently creates active accounts, and login does not check `isActive`.
- **Per-user data.** Dashboards and enrollment currently work on shared data and a fixed student ID.
- **Editable quizzes.** The quiz screen ships three built-in questions; wire it to `Quiz` and `Question`.

## ✦ Team

| Contributor | Focus |
|---|---|
| **Arfa Ashraf https://github.com/arfa-ashraf** | Class diagram, user hierarchy, login and registration screens |
| **Maryam Amin https://github.com/aminmaryam** | Course hierarchy, quiz / assignment / grading classes, teacher dashboard |
| **Sumaiya Abubakar** | Student helpers, admin helpers, student and admin dashboards |

<div align="center">

<img src="assets/divider.svg" alt="" width="100%">

<sub>Built with C++ and raylib · SAM — Smart Academic Manager</sub>

</div>
