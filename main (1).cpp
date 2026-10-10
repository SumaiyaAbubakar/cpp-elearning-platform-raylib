#include <iostream>
#include <string>
#include <stdexcept>
// #include "gui.h"

using namespace std;

/* ===== FORWARD DECLARATIONS ===== */
class Course;
class Submission;
class Quiz;
class Assignment;
class Badge;
class ToDoList;
class ZoomLink;
class TASK;      // forward declare TASK before ToDoList uses it


/* ================= USER ================= */
class User 
{
protected:                          
    string userID;                     
    string name;
    string email;
    string password;
    string role;
    bool isActive;
    string registrationDate;

    static int totalUsers;  

public:
    User(string userID, string name, string email, string password, string role);
    virtual bool login(string email, string password) = 0;
    virtual void viewProfile() = 0;
    void registeredUser();
    void updateProfile(string newName, string newEmail, string newPhone);
    void changePassword(string newPass);
    static int getTotalUsers();
    string getUserID();
};

int User::totalUsers = 0;

User::User(string userID, string name, string email, string password, string role) 
{
    this->name = name;
    this->email = email;
    this->password = password;
    this->role = role;
    this->isActive = true;
    this->registrationDate = "2025-01-01";
    totalUsers++;
    this->userID = userID;
}

void User::registeredUser() 
{
    cout << "User registered: " << name << " (ID: " << userID << ")" << endl;
}

void User::updateProfile(string newName, string newEmail, string newPhone) 
{
    name = newName;
    email = newEmail;
    
    cout << "Profile updated." << endl;
}

void User::changePassword(string newPass) 
{
    password = newPass;
    cout << "Password changed." << endl;
}

int User::getTotalUsers() 
{
    return totalUsers;
}

string User::getUserID(){return userID;}


/* ================= TEACHER ================= */
class Teacher : public User 
{
private:
    string  teacherID;          // FIX: was int, but userID is string
    Course* courses[50];
    int     courseCount;
    string  zoomLink;
    string  section;

public:
    Teacher(string name, string email, string password);

    bool login(string email, string password) override;
    void viewProfile() override;

    void addCourse(Course* course);
    void uploadMaterial(Course* course, string filePath);
    void createQuiz(Course* course);
    void createAssignment(Course* course);
    void gradeSubmission(class Submission* sub, float mark);
    void viewGradebook(int courseID);
    void submitCourseForApproval(Course* course);
};

Teacher::Teacher(string name, string email, string password)
    : User("", name, email, password, "Teacher")   // FIX: pass empty string for userID (was uninitialized)
{
    teacherID = userID;
    courseCount = 0;
}

bool Teacher::login(string email, string password) 
{
    if (this->email == email && this->password == password) 
    {
        cout << "Teacher login: " << name << endl;
        return true;
    }
    return false;
}

void Teacher::viewProfile() 
{
    cout << "=== Teacher Profile ===" << endl;
    cout << "ID: " << teacherID << endl;
    cout << "Name: " << name << endl;
    cout << "Section: " << section << endl;
    cout << "Zoom: " << zoomLink << endl;
}

void Teacher::addCourse(Course* course) 
{
    courses[courseCount] = course;
    courseCount++;
}

// NOTE: Implementations of uploadMaterial, createQuiz, createAssignment,
// gradeSubmission, viewGradebook, submitCourseForApproval are defined
// after Course/Submission are fully declared below.


// Student class is declared later, after Badge/ToDoList/ZoomLink are fully defined.
// Forward-declare it here so Course and other classes can use Student* pointers.
class Student;

// Student constructor and method implementations are placed after
// Badge, ToDoList, ZoomLink are fully defined (see below).


/* ================= ADMIN ================= */
class Admin : public User 
{
private:
    string  adminID;            // FIX: was int, but userID is string
    Course* managedCourses[100];
    int     managedCount;

public:
    Admin(string name, string email, string password);

    bool login(string email, string password) override;
    void viewProfile() override;

    void approveCourse(int courseID);
    void rejectCourse(int courseID);
    void approveRegistration(int userID);
    void removeUser(int userID);
    void resetPassword(int userID);
    void monitorEnrollments();
    void manageSubscriptions();
};

Admin::Admin(string name, string email, string password)
    : User("", name, email, password, "Admin")  // FIX: pass empty string for userID
{
    adminID = userID;
    managedCount = 0;           // FIX: initialize managedCount
}

bool Admin::login(string email, string password) 
{
    if (this->email == email && this->password == password) 
    {
        cout << "Admin login: " << name << endl;
        return true;
    }
    return false;
}

void Admin::viewProfile() 
{
    cout << "=== Admin Profile ===" << endl;
    cout << "ID: " << adminID << endl;
    cout << "Name: " << name << endl;
}

void Admin::approveCourse(int courseID) 
{
    cout << "Course " << courseID << " approved." << endl;
}

void Admin::rejectCourse(int courseID) 
{
    cout << "Course " << courseID << " rejected." << endl;
}

void Admin::approveRegistration(int userID) 
{
    cout << "Registration approved for user " << userID << endl;
}

void Admin::removeUser(int userID) 
{
    cout << "User " << userID << " removed." << endl;
}

void Admin::resetPassword(int userID) 
{
    cout << "Password reset for user " << userID << endl;
}

void Admin::monitorEnrollments() 
{
    cout << "Total users: " << getTotalUsers() << endl;
}

void Admin::manageSubscriptions() 
{
    cout << "Managing subscriptions..." << endl;
}


/* ===== MORE FORWARD DECLARATIONS ===== */

// =============================================================
// CLASS 1 : Material
// =============================================================
class CourseMaterial {

private:
    int    materialID;
    string title;
    string fileType;
    string filePath;
    string uploadDate;
    int    courseID;

    static int nextID;

public:
    CourseMaterial(string title, string fileType, string filePath, string uploadDate, int courseID)
    {
        this->materialID  = nextID++;
        this->title       = title;
        this->fileType    = fileType;
        this->filePath    = filePath;
        this->uploadDate  = uploadDate;
        this->courseID    = courseID;
    }

    void display() const
    {
        cout << "-----------------------------\n";
        cout << "Material ID  : " << materialID  << "\n";
        cout << "Title        : " << title        << "\n";
        cout << "File Type    : " << fileType     << "\n";
        cout << "Upload Date  : " << uploadDate   << "\n";
        cout << "Course ID    : " << courseID     << "\n";
        cout << "-----------------------------\n";
    }

    void download() const
    {
        cout << "Downloading \"" << title << "\" from: " << filePath << "\n";
    }

    int getMaterialID() const { return materialID; }
    int getCourseID()   const { return courseID;   }
};

int CourseMaterial::nextID = 1;

class Student;


/* ================= COURSE ================= */
class Course 
{
protected:
    string     courseID;
    string  title;
    string  description;
    float   price;
    int     prerequisiteID;   
    bool    isApproved;
    int     teacherID;

    CourseMaterial*   materials[50];
    Quiz*       quizzes[50];
    Assignment* assignments[50];
    int materialCount;
    int quizCount;
    int assignmentCount;
    string status;
    static int totalCourses;

public:
    Course(string courseID, string title, string desc, float price, int teacherID, int prereqID = 0);

    virtual void displayInfo() = 0;
    virtual void enroll(Student* s) = 0;

    bool   getApprovalStatus();
    void   approve();
    void   addMaterial(CourseMaterial* m);
    void   addQuiz(Quiz* q);
    void   addAssignment(Assignment* a);

    string    getCourseID();
    string getTitle();
    int    getPrerequisiteID();
    float  getPrice();
    void setStatus(string stat);
};


/* ===== COURSE IMPLEMENTATION ===== */
int Course::totalCourses = 0;

Course::Course(string courseID, string title, string desc, float price, int teacherID, int prereqID) 
{
    totalCourses++;
    this->courseID       = courseID;
    this->title          = title;
    this->description    = desc;
    this->price          = price;
    this->teacherID      = teacherID;
    this->prerequisiteID = prereqID;
    this->isApproved     = false;
    this->materialCount   = 0;
    this->quizCount       = 0;
    this->assignmentCount = 0;
}

bool Course::getApprovalStatus() 
{
    return isApproved;
}

void Course::approve() 
{
    isApproved = true;
    cout << "Course approved: " << title << endl;
}

void Course::addMaterial(CourseMaterial* m) 
{
    materials[materialCount] = m;
    materialCount++;
    cout << "Material added to: " << title << endl;
}

void Course::addQuiz(Quiz* q) 
{
    quizzes[quizCount] = q;
    quizCount++;
}

void Course::addAssignment(Assignment* a) 
{
    assignments[assignmentCount] = a;
    assignmentCount++;
}

string Course::getCourseID()       
{ 
    return courseID; 
}

string Course::getTitle()          
{ 
    return title; 
}

int Course::getPrerequisiteID() 
{ 
    return prerequisiteID; 
}

float Course::getPrice()          
{ 
    return price; 
}

void Course::setStatus(string stat){
    status = stat;
}


/* ================= FREE COURSE ================= */
class FreeCourse : public Course 
{
private:
    string accessLevel;

public:
    FreeCourse(string courseID, string title, string desc, float price, int teacherID, int prereqID = 0);

    void displayInfo() override;
    void enroll(Student* s) override;
};


/* ===== FREE COURSE IMPLEMENTATION ===== */
FreeCourse::FreeCourse(string courseID, string title, string desc, float price, int teacherID, int prereqID)
    : Course(courseID, title, desc, 0.0f, teacherID, prereqID)
{
    this->accessLevel = "public";
}

void FreeCourse::displayInfo() 
{
    cout << "=== FREE COURSE ===" << endl;
    cout << "ID:          " << courseID       << endl;
    cout << "Title:       " << title          << endl;
    cout << "Description: " << description    << endl;
    cout << "Price:       FREE"               << endl;
    cout << "Approved:    " << (isApproved ? "Yes" : "No") << endl;
    cout << "Access:      " << accessLevel    << endl;
}

// FreeCourse::enroll defined after Student is fully declared (it calls s->enroll)


/* ================= PAID COURSE ================= */
class PaidCourse : public Course 
{
private:
    string subscriptionPlan;
    bool   paymentRequired;

public:
    PaidCourse(string courseID, string title, string desc, float price,   // FIX: added missing courseID param
               int teacherID, string plan, int prereqID = 0);

    void displayInfo() override;
    void enroll(Student* s) override;
    bool processPayment(Student* s);
};


/* ===== PAID COURSE IMPLEMENTATION ===== */
PaidCourse::PaidCourse(string courseID, string title, string desc, float price,   // FIX: added missing courseID param
                       int teacherID, string plan, int prereqID)
    : Course(courseID, title, desc, price, teacherID, prereqID)
{
    this->subscriptionPlan = plan;
    this->paymentRequired  = true;
}

void PaidCourse::displayInfo() 
{
    cout << "=== PAID COURSE ===" << endl;
    cout << "ID:           " << courseID          << endl;
    cout << "Title:        " << title             << endl;
    cout << "Description:  " << description       << endl;
    cout << "Price:        Rs." << price          << endl;
    cout << "Plan:         " << subscriptionPlan  << endl;
    cout << "Approved:     " << (isApproved ? "Yes" : "No") << endl;
}

bool PaidCourse::processPayment(Student* s) 
{
    cout << "Processing payment of Rs." << price << "..." << endl;
    cout << "Payment successful!" << endl;
    return true;
}

// PaidCourse::enroll defined after Student is fully declared (it calls s->enroll)


//====================  Content, grading, enrollment, and prerequisite ===================================




// =============================================================
// CLASS 2 : Question
// =============================================================
class Question {

private:
    int    questionID;
    string text;
    string options[4];
    int    correctAnswerIndex;
    int    marks;

    static int nextID;

public:
    Question(){
        this->questionID         = 0;
        this->correctAnswerIndex = 0;
        this->marks              = 0;
    }

    Question(string text, string optA, string optB,
             string optC, string optD,
             int correctIndex, int marks)
    {
        this->questionID          = nextID++;
        this->text                = text;
        this->options[0]          = optA;
        this->options[1]          = optB;
        this->options[2]          = optC;
        this->options[3]          = optD;
        this->correctAnswerIndex  = correctIndex;
        this->marks               = marks;
    }

    int getCorrectAnswer() const { return correctAnswerIndex; }

    bool checkAnswer(int selectedIndex) const
    {
        return (selectedIndex == correctAnswerIndex);
    }

    void display() const
    {
        cout << "\nQ" << questionID << ": " << text << "\n";
        for (int i = 0; i < 4; i++) {
            cout << "  " << (char)('A' + i) << ") " << options[i] << "\n";
        }
        cout << "  [Marks: " << marks << "]\n";
    }

    int    getMarks()      const { return marks;      }
    int    getQuestionID() const { return questionID;  }
    string getText()       const { return text;        }
};

int Question::nextID = 1;


// =============================================================
// CLASS 3 : Quiz
// =============================================================
class Quiz {

private:
    int      quizID;
    string   title;
    int      courseID;

    static const int MAX_QUESTIONS = 50;
    Question questions[MAX_QUESTIONS];
    int      questionCount;

    int      totalMarks;
    int      durationMinutes;

    static int nextID;

public:
    Quiz(string title, int courseID, int durationMinutes)
    {
        this->quizID          = nextID++;
        this->title           = title;
        this->courseID        = courseID;
        this->totalMarks      = 0;
        this->durationMinutes = durationMinutes;
        this->questionCount   = 0;
    }

    void addQuestion(Question q)
    {
        if (questionCount >= MAX_QUESTIONS) {
            cout << "Quiz is full. Cannot add more questions.\n";
            return;
        }
        questions[questionCount] = q;
        questionCount++;
        totalMarks += q.getMarks();
    }

    float autoGrade(int answers[], int answerCount)
    {
        if (answerCount != questionCount) {
            cout << "Answer count does not match question count!\n";
            return 0;
        }

        float scored = 0;
        for (int i = 0; i < questionCount; i++) {
            if (questions[i].checkAnswer(answers[i])) {
                scored += questions[i].getMarks();
            }
        }
        return scored;
    }

    float attempt()
    {
        cout << "\n===== QUIZ: " << title << " (" << durationMinutes << " min) =====\n";

        int studentAnswers[MAX_QUESTIONS];

        for (int i = 0; i < questionCount; i++) {
            questions[i].display();

            char choice;
            cout << "  Your answer (A/B/C/D): ";
            cin  >> choice;

            int index = toupper(choice) - 'A';

            if (index < 0 || index > 3) {
                cout << "  Invalid input — marking as wrong.\n";
                index = -1;
            }

            studentAnswers[i] = index;
        }

        float score = autoGrade(studentAnswers, questionCount);
        cout << "\nYour score: " << score << " / " << totalMarks << "\n";
        return score;
    }

    void display() const
    {
        cout << "Quiz [" << quizID << "] " << title
             << " | Course: " << courseID
             << " | Total Marks: " << totalMarks
             << " | Duration: " << durationMinutes << " min\n";
    }

    int getQuizID()     const { return quizID;      }
    int getCourseID()   const { return courseID;    }
    int getTotalMarks() const { return totalMarks;  }
};

int Quiz::nextID = 1;


// =============================================================
// CLASS 4 : Assignment
// =============================================================
class Assignment {

private:
    int    assignmentID;
    string title;
    string description;
    string deadline;
    int    maxMarks;
    int    courseID;

    static int nextID;

public:
    Assignment(string title, string description,
               string deadline, int maxMarks, int courseID)
    {
        this->assignmentID = nextID++;
        this->title        = title;
        this->description  = description;
        this->deadline     = deadline;
        this->maxMarks     = maxMarks;
        this->courseID     = courseID;
    }

    void display() const
    {
        cout << "-----------------------------\n";
        cout << "Assignment ID  : " << assignmentID << "\n";
        cout << "Title          : " << title        << "\n";
        cout << "Description    : " << description  << "\n";
        cout << "Deadline       : " << deadline     << "\n";
        cout << "Max Marks      : " << maxMarks     << "\n";
        cout << "Course ID      : " << courseID     << "\n";
        cout << "-----------------------------\n";
    }

    int    getAssignmentID() const { return assignmentID; }
    int    getCourseID()     const { return courseID;     }
    int    getMaxMarks()     const { return maxMarks;     }
    string getTitle()        const { return title;        }
    string getDeadline()     const { return deadline;     }
};

int Assignment::nextID = 1;


// =============================================================
// CLASS 5 : Submission
// =============================================================
class Submission {

private:
    int    submissionID;
    int    studentID;
    int    assignmentID;
    string filePath;
    string submittedDate;
    float  grade;
    bool   isGraded;

    static int nextID;

public:
    Submission(int studentID, int assignmentID, string filePath, string submittedDate)
    {
        this->submissionID   = nextID++;
        this->studentID      = studentID;
        this->assignmentID   = assignmentID;
        this->filePath       = filePath;
        this->submittedDate  = submittedDate;
        this->grade          = 0;
        this->isGraded       = false;
    }

    void submit()
    {
        cout << "Submission [" << submissionID << "] recorded.\n";
        cout << "File: " << filePath << "\n";
        cout << "Submitted on: " << submittedDate << "\n";
    }

    void markGraded(float mark)
    {
        this->grade    = mark;
        this->isGraded = true;
        cout << "Submission " << submissionID
             << " graded: " << grade << "\n";
    }

    float getGrade() const
    {
        if (!isGraded) {
            cout << "This submission has not been graded yet.\n";
            return 0;
        }
        return grade;
    }

    void display() const
    {
        cout << "-----------------------------\n";
        cout << "Submission ID  : " << submissionID  << "\n";
        cout << "Student ID     : " << studentID     << "\n";
        cout << "Assignment ID  : " << assignmentID  << "\n";
        cout << "Submitted On   : " << submittedDate << "\n";
        cout << "Graded         : " << (isGraded ? "Yes" : "No") << "\n";
        if (isGraded)
            cout << "Grade          : " << grade << "\n";
        cout << "-----------------------------\n";
    }

    int  getSubmissionID() const { return submissionID; }
    int  getStudentID()    const { return studentID;    }
    int  getAssignmentID() const { return assignmentID; }
    bool getIsGraded()     const { return isGraded;     }
};

int Submission::nextID = 1;


// =============================================================
// CLASS 6 : GradeBook
// =============================================================
class GradeBook {

private:
    int courseID;

    static const int MAX_STUDENTS = 100;
    int   studentIDs[MAX_STUDENTS];
    float studentGrades[MAX_STUDENTS];
    int   studentCount;

    float classAverage;

public:
    GradeBook(int courseID)
    {
        this->courseID     = courseID;
        this->classAverage = 0;
        this->studentCount = 0;
    }

    void addGrade(int studentID, float mark)
    {
        for (int i = 0; i < studentCount; i++) {
            if (studentIDs[i] == studentID) {
                studentGrades[i] = mark;
                recalculateAverage();
                cout << "Grade " << mark << " updated for student " << studentID << "\n";
                return;
            }
        }

        if (studentCount >= MAX_STUDENTS) {
            cout << "Gradebook is full.\n";
            return;
        }

        studentIDs[studentCount]    = studentID;
        studentGrades[studentCount] = mark;
        studentCount++;
        recalculateAverage();
        cout << "Grade " << mark << " recorded for student " << studentID << "\n";
    }

private:
    void recalculateAverage()
    {
        if (studentCount == 0) {
            classAverage = 0;
            return;
        }
        float total = 0;
        for (int i = 0; i < studentCount; i++) {
            total += studentGrades[i];
        }
        classAverage = total / (float)studentCount;
    }

public:
    float getClassAverage() const { return classAverage; }

    float getStudentGrade(int studentID) const
    {
        for (int i = 0; i < studentCount; i++) {
            if (studentIDs[i] == studentID) {
                return studentGrades[i];
            }
        }
        cout << "Student " << studentID << " not found in gradebook.\n";
        return -1;
    }

    void display() const
    {
        cout << "\n===== GRADEBOOK (Course " << courseID << ") =====\n";
        if (studentCount == 0) {
            cout << "No grades recorded yet.\n";
            return;
        }
        for (int i = 0; i < studentCount; i++) {
            cout << "  Student " << studentIDs[i]
                 << " : " << studentGrades[i] << "\n";
        }
        cout << "  Class Average: " << classAverage << "\n";
        cout << "==========================================\n";
    }

    int getCourseID() const { return courseID; }
};


// =============================================================
// CLASS 7 : Enrollment
// =============================================================
class Enrollment {

private:
    int    enrollmentID;
    int    studentID;
    int    courseID;
    bool   paymentStatus;
    string enrolledDate;
    bool   isConfirmed;

    static int nextID;

public:
    Enrollment(int studentID, int courseID, string enrolledDate, bool paymentStatus = false)
    {
        this->enrollmentID   = nextID++;
        this->studentID      = studentID;
        this->courseID       = courseID;
        this->enrolledDate   = enrolledDate;
        this->paymentStatus  = paymentStatus;
        this->isConfirmed    = false;
    }

    void confirm(){
        if (!paymentStatus) {
            cout << "Warning: Payment not done. Confirming anyway (free course?).\n";
        }
        isConfirmed = true;
        cout << "Enrollment [" << enrollmentID << "] confirmed.\n";
        cout << "Student " << studentID
             << " enrolled in Course " << courseID << "\n";
    }

    void cancel()
    {
        isConfirmed   = false;
        paymentStatus = false;
        cout << "Enrollment [" << enrollmentID << "] cancelled.\n";
    }

    bool isPaid() const { return paymentStatus; }

    void markPaid()
    {
        paymentStatus = true;
        cout << "Payment recorded for enrollment " << enrollmentID << "\n";
    }

    void display() const
    {
        cout << "-----------------------------\n";
        cout << "Enrollment ID  : " << enrollmentID << "\n";
        cout << "Student ID     : " << studentID    << "\n";
        cout << "Course ID      : " << courseID     << "\n";
        cout << "Enrolled On    : " << enrolledDate << "\n";
        cout << "Paid           : " << (paymentStatus ? "Yes" : "No") << "\n";
        cout << "Confirmed      : " << (isConfirmed  ? "Yes" : "No") << "\n";
        cout << "-----------------------------\n";
    }

    int  getEnrollmentID() const { return enrollmentID; }
    int  getStudentID()    const { return studentID;    }
    int  getCourseID()     const { return courseID;     }
    bool getIsConfirmed()  const { return isConfirmed;  }
};

int Enrollment::nextID = 1;


// =============================================================
// CLASS 8 : PrerequisiteChecker
// =============================================================
class PrerequisiteChecker {

private:
    static const int MAX_COURSES = 100;
    int completedCourseIDs[MAX_COURSES];
    int completedCount;

public:
    PrerequisiteChecker(){
        this->completedCount = 0;
    }

    void addCompleted(int courseID) {
        if (completedCount >= MAX_COURSES) {
            cout << "Completed course list is full.\n";
            return;
        }
        completedCourseIDs[completedCount] = courseID;
        completedCount++;
        cout << "Course " << courseID
             << " marked as completed in PrerequisiteChecker.\n";
    }

    bool isEligible(int studentID, int prereqID) const
    {
        for (int i = 0; i < completedCount; i++) {
            if (completedCourseIDs[i] == prereqID) {
                return true;
            }
        }
        return false;
    }

    bool check(int studentID, int prereqID) const 
    {
        if (prereqID <= 0) {
            cout << "No prerequisite required. Student " << studentID
                 << " is eligible.\n";
            return true;
        }

        if (isEligible(studentID, prereqID)) {
            cout << "Student " << studentID
                 << " has completed prerequisite course " << prereqID
                 << ". Eligible to enroll.\n";
            return true;
        } else {
            cout << "Student " << studentID
                 << " has NOT completed prerequisite course " << prereqID
                 << ". Cannot enroll yet.\n";
            return false;
        }
    }

    void displayCompleted() const
    {
        cout << "Completed courses: ";
        if (completedCount == 0) {
            cout << "None\n";
            return;
        }
        for (int i = 0; i < completedCount; i++) {
            cout << completedCourseIDs[i] << " ";
        }
        cout << "\n";
    }
};


//================================    Student, helper and admin helper classes =====================================


// =============================================================
// CLASS : Badge
// =============================================================
class Badge{
private:
int badgeID;
string name;
float minPercentage;
string iconlabel;

static int nextID;
public:
Badge(){
    badgeID= nextID++;
    name="none";
    minPercentage=0.0;
    iconlabel="none";
}
Badge(string name, float percentage, string label):name(name),minPercentage(percentage), iconlabel(label){
    badgeID=nextID++;
}

void display()const{
     cout << "==============================\n";
    cout << "  Badge ID    : " << badgeID   << "\n";
    cout << "  Badge Name  : " << name      << "\n";
    cout << "  Min %       : " << minPercentage << "%\n";
    cout << "  Icon        : " << iconlabel << "\n";
    cout << "==============================\n";
}

int getID()const{
    return badgeID;
}
string getname()const{
    return name;
}
float getpercentage()const{
    return minPercentage;
}
string getlabel()const{
    return iconlabel;
}

void setid(int id){
    badgeID=id;
}
void setname(string n){
    name=n;
}
void setpercentage(float p){
    minPercentage=p;
}
void setLabel(string l){
    iconlabel=l;
}

void award(float overallPercentage){
if(overallPercentage>=90.0f){
    name             = "PLATINUM";
    minPercentage    = 90.0f;
    iconlabel        = "[***]";
}
else if(overallPercentage>=75.0f){
    name             = "GOLD";
    minPercentage    = 75.0f;
    iconlabel        = "[** ]";
}
else if(overallPercentage>=60.0f){
    name             = "SILVER";
    minPercentage    = 60.0f;
    iconlabel        = "[*  ]";
} else if(overallPercentage>=40.0f){
    name             = "BRONZE";
    minPercentage    = 40.0f;
    iconlabel        = "[ . ]";
} else{
 name                = "NONE";
    minPercentage    = 0.0f;
    iconlabel        = "[ __ ]";
    cout<<"No badge rewarded. Low percentage!"<<endl;
    return ;
}
cout<<"Bagde rewarded : "  <<iconlabel <<endl;
}

// static Badge determineBadge(float percentage){
//     if (percentage >= 90.0f) return Badge("Platinum", 90.0f, "[***]");
//     if (percentage >= 75.0f) return Badge("Gold",     75.0f, "[** ]");
//     if (percentage >= 60.0f) return Badge("Silver",   60.0f, "[*  ]");
//     if (percentage >= 40.0f) return Badge("Bronze",   40.0f, "[ . ]");
//     return Badge("None", 0.0f, "[_]");
// }
};
int Badge::nextID=1;


// =============================================================
// CLASS : TASK
// =============================================================
class TASK{
private:
int taskId;
string title;
string dueDate;
bool isDone;
string priority;
static int nextID;
public:
TASK(){
    taskId=nextID++;
    title="none";
    dueDate="none";
    isDone=false;
    priority="none";
}
TASK(string title, string date, string priority):title(title),dueDate(date),priority(priority){
   taskId=nextID++;
   isDone=false;   // FIX: initialize isDone
}
void markDone(){
     if (isDone) {
        cout << "Task '" << title << "' is already marked as done.\n";
        return;
    }
    isDone = true;
    cout << "Task '" << title << "' marked as done.\n";
}
void edit(const string &newtitle, const string &newdate){
    if (isDone) {
        cout << "Cannot edit a completed task.\n";
        return;
    }
    title   = newtitle;
    dueDate = newdate;
    cout << "Task updated successfully.\n";
}

int priorityRank()const{
    if (priority == "High")   return 3;
    if (priority == "Medium") return 2;
    if (priority == "Low")    return 1;
    return 0;
}
void display()const{
    cout<< "Task ID:  "  <<taskId<<endl;
    cout<<"Task title:  "<< title<<endl;
    cout<<"Due Date:    "<<dueDate<<endl;
    cout<<"Priority:    "<<priority<<endl;
}
int getid()const{
    return taskId;
}
string gettitle()const{
    return title;
}
string getdate()const{
    return dueDate;
}
bool getstatus()const{
    return isDone;
}
string getPriority()const{
    return priority;
}
void setpriority(string prior){
    priority=prior;
}
void setid(int id){
    taskId=id;
}
};
int TASK::nextID=1;


// =============================================================
// CLASS : ToDoList
// =============================================================
const int MAX_TASK=100;
class ToDoList{
private:
int studentId;
TASK tasks[MAX_TASK];
int taskcount;
public:
ToDoList(){
    studentId=0;
    taskcount=0;
}
ToDoList(int id):studentId(id), taskcount(0){}

int findtaskIndex(int taskID)const{
for(int i=0;i<taskcount;i++){
    if(tasks[i].getid()==taskID){
        return i;
    }
   
}
 return -1;
}
bool addTasks(const TASK &task){
    if(taskcount>=MAX_TASK){
        cout<<"To Do List is full. Cannot add more tasks"<<endl;
        return false;
    }else{
        tasks[taskcount++]=task;
        cout<<"Task added successfully!"<<endl;
        return true;
    }
}

bool removeTask(const int id){
  int index= findtaskIndex(id);
  if(index==-1){
    cout<<"Task ID " << id << " not found.\n";
        return false;
  }
   for (int i = index; i < taskcount - 1; i++) {
        tasks[i] = tasks[i + 1];
    }
    taskcount--;
    cout << "Task removed successfully.\n";
    return true;
}
void viewPending()const{
     cout << "--- Pending Tasks (Student " << studentId << ") ---"<<endl;
    bool found = false;
    for (int i = 0; i < taskcount; i++) {
        if (!tasks[i].getstatus()) {
            tasks[i].display();
            found = true;
        }
        
} if (!found) cout << "  No pending tasks.;"<<endl;
}
bool markDone(int id){
int index = findtaskIndex(id);
    if (index == -1) {
        cout << "Task ID " << id << " not found.\n";
        return false;
    }
    tasks[index].markDone();
    return true;
}
void viewall(){
    cout << "=== All Tasks (Student " << studentId << ") ====;"<<endl;

    if (taskcount == 0) {
         cout << "  No tasks.\n"<<endl;
          return;
         }
    for (int i = 0; i < taskcount; i++){ 
    tasks[i].display();
}
}
void sortbyPriority(){
    for (int i = 1; i <taskcount; i++) {
        TASK key = tasks[i];
        int j = i - 1;
        while (j>=0 &&tasks[j].priorityRank()< key.priorityRank())
        {
         tasks[j+1]=tasks[j];
         j--;
        }
        tasks[j+1]=key;
    }
    cout << "Tasks sorted by priority (High first).\n";
}
int getPendingCount(){
    int count = 0;
    for (int i = 0; i < taskcount; i++)
     if (!tasks[i].getstatus()){ 
        count++;
     }
    return count;
}
int getid(){
    return studentId;
}
};


// =============================================================
// CLASS : ZoomLink
// =============================================================
class ZoomLink{
private:
int linkID;
string teachername;
string section;
string coursetitle;
string url;
string scheduledTime;
static int nextid;
public:
ZoomLink(){
    linkID=0;
    teachername="none";
     coursetitle="none";
     section="none";
     scheduledTime="0000:00";
     url="none";
     nextid++;
}
ZoomLink(int id, string name, string sec, string title, string url, string time):linkID(id),teachername(name),section(sec),coursetitle(title),url(url),scheduledTime(time){
    nextid++;
}
int getLinkID() const {
     return linkID;
     }
    string getTeacherName() const
    { return teachername;
     
    }
    string getSection() const { 
        return section;
     }
    string getCourseTitle()  const { 
        return coursetitle;
     }
    string getUrl()   const {
         return url;
         }
    string getScheduledTime()const { 
        return scheduledTime;
     }
     void seturl(string u){
        url=u;
     }
     void setTime(string t){
        scheduledTime=t;
     }
 
     void display(){
      cout<<"===================================="<<endl;
      cout<<" Link Id:       "<< linkID <<endl;
      cout<<" Course:        "<< coursetitle<<endl;
      cout << "  Teacher      : " << teachername  << endl;
     cout << "  Section      : " << section       << endl;
     cout << "  Scheduled    : " << scheduledTime << endl;
     cout << "  URL          : " << url           << endl;
     cout << "-------------------------------\n";
     }
     
     void openlink(){
      if(url.empty()){
         cout<<"No URL set for this zoom link"<<endl;
         return;
      }
      cout<<" Opening link:"<< url<<endl;
       cout << "(In GUI mode this would launch your browser.)\n";
     }

     bool operator >(const string &currentTime)const{
      return scheduledTime>currentTime;
     }

};
int ZoomLink::nextid=1;


// =============================================================
// CLASS : Student  (full definition — Badge/ToDoList/ZoomLink now complete)
// =============================================================
class Student : public User {       
    string   studentID;

    Course*  enrolledCourses[50];
    int      enrolledCount;

    int      courseIDs[50];
    float    gradeValues[50];
    int      gradeCount;

    float    overallPercent;
    Badge    badge;
    ToDoList todoList;

    ZoomLink zoomLinks[20];
    int      zoomCount;

public:
    Student(string name, string email, string password);

    void enroll(Course* course);
    void addGrade(int courseID, float grade);
    void viewGrades();
    void attemptQuiz(Quiz* quiz);
    void submitAssignment(Assignment* assign);
    void earnBadge();
    void viewZoomLinks();

    bool login(string email, string password) override;
    void viewProfile() override;
};

// Now define Student constructor and methods (Badge, ToDoList, ZoomLink are complete)
Student::Student(string name, string email, string password)
    : User("", name, email, password, "Student")   // FIX: pass empty string for userID
{
    studentID      = userID;
    overallPercent = 0.0f;
    enrolledCount  = 0;
    gradeCount     = 0;
    zoomCount      = 0;
}

bool Student::login(string email, string password) 
{
    if (this->email == email && this->password == password) 
    {
        cout << "Student login successful: " << name << endl;
        return true;
    }
    cout << "Invalid credentials." << endl;
    return false;
}

void Student::viewProfile() 
{
    cout << "=== Student Profile ===" << endl;
    cout << "ID: " << studentID << endl;
    cout << "Name: " << name << endl;
    cout << "Email: " << email << endl;
    cout << "Role: " << role << endl;
    cout << "Overall %: " << overallPercent << endl;
}

void Student::enroll(Course* course) 
{
    enrolledCourses[enrolledCount] = course;
    enrolledCount++;
    cout << name << " enrolled in: " << course->getTitle() << endl;
}

void Student::addGrade(int courseID, float grade) 
{
    for (int i = 0; i < gradeCount; i++) 
    {
        if (courseIDs[i] == courseID) 
        {
            gradeValues[i] = grade;
            return;
        }
    }
    courseIDs[gradeCount]   = courseID;
    gradeValues[gradeCount] = grade;
    gradeCount++;
}

void Student::viewGrades() 
{
    cout << "Grades for " << name << ":" << endl;
    for (int i = 0; i < gradeCount; i++) 
    {
        cout << "  Course " << courseIDs[i]
             << ": "        << gradeValues[i] << endl;
    }
}

void Student::earnBadge() 
{
    badge.award(overallPercent);   // FIX: award takes float, not Student*
}

void Student::viewZoomLinks() 
{
    for (int i = 0; i < zoomCount; i++) 
    {
        zoomLinks[i].display();
    }
}

void Student::attemptQuiz(Quiz* quiz)
{
    quiz->attempt();
}

void Student::submitAssignment(Assignment* assign)
{
    cout << "Submitting assignment: " << assign->getTitle() << endl;
}

// Deferred Course::enroll implementations (need complete Student type)
void FreeCourse::enroll(Student* s) 
{
    cout << "Enrolling in free course: " << title << endl;
    s->enroll(this);
    cout << "Enrolled successfully!" << endl;
}

void PaidCourse::enroll(Student* s) 
{
    cout << "Attempting to enroll in paid course: " << title << endl;

    if (!isApproved) 
    {
        cout << "Cannot enroll — course not yet approved by Admin." << endl;
        return;          
    }

    bool paid = processPayment(s);

    if (paid) 
    {
        s->enroll(this);
        cout << "Enrolled in paid course successfully!" << endl;
    } 
    else 
    {
        cout << "Enrollment failed — payment unsuccessful." << endl;
    }
}


// Now define Teacher methods that need Submission/Course fully defined
void Teacher::uploadMaterial(Course* course, string filePath) 
{
    cout << "Material uploaded: " << filePath
         << " to course: "        << course->getTitle() << endl;
}

void Teacher::createQuiz(Course* course) 
{
    cout << "Quiz created for: " << course->getTitle() << endl;
}

void Teacher::createAssignment(Course* course) 
{
    cout << "Assignment created for: " << course->getTitle() << endl;
}

void Teacher::gradeSubmission(Submission* sub, float mark) 
{
    sub->markGraded(mark);
}

void Teacher::viewGradebook(int courseID) 
{
    cout << "Viewing gradebook for course " << courseID << endl;
}

void Teacher::submitCourseForApproval(Course* course) 
{
    cout << "Course submitted for approval: "
         << course->getTitle() << endl;
}


// =============================================================
// CLASS : IApprovable
// =============================================================
class IApprovable{
protected:
static const string APPROVED;
static const string PENDING;
static const string REJECTED;

    virtual void approve()=0;
    virtual void reject()=0;
    virtual string getStatus()=0;
    virtual ~IApprovable() = default;
};
const string IApprovable::APPROVED = "Approved";   // FIX: was = 0 (int assigned to string)
const string IApprovable::REJECTED = "Rejected";   // FIX: was = 0
const string IApprovable::PENDING  = "Pending";    // FIX: was = 0


// =============================================================
// CLASS : RegistrationApproval
// =============================================================
class RegistrationApproval:public IApprovable{
private:
static int requestCounter;

    int requestID;
    int userID;
    string userRole;
    string status;
    string requestDate;

    string validateRole(string role){
        if(role!="Student" && role!="Teacher")
        throw invalid_argument("Invalid role!");
        return role;
    }

    string intToString(int x) const {
        if (x == 0) return "0";

        string s = "";
        while (x > 0) {
            char digit = '0' + (x % 10);
            s = digit + s;
            x /= 10;
        }
        return s;
    }

    public:
    RegistrationApproval(int uID, string role){
        requestID=++requestCounter;
        userID = uID ;
        userRole = validateRole(role);
        status = PENDING;
        requestDate = "N/A";
    }
    void approve()override{
        if(status!=PENDING){
             throw runtime_error("Request " + intToString(requestID) + " already processed");
        }                           // FIX: closing brace was misplaced; status=APPROVED must be outside the if
        status = APPROVED;
    }
    void reject()override{
        if (status != PENDING)
            throw runtime_error("Request " + intToString(requestID) + " already processed");
        status = REJECTED;
    }
    string  getStatus()override{
return status;
    }

     int getRequestID() const { 
        return requestID;
     }
    int getUserID() const {
         return userID;
         }
    string getUserRole() const { 
        return userRole; 
    }

    void print() const {
        cout << "Request[" << requestID << "] "<<endl;
        cout << "User:" << userID <<endl;
        cout << " Role:" << userRole<<endl;
        cout  << " Status:" << status << endl;

    }
};

int RegistrationApproval::requestCounter=0;


// =============================================================
// CLASS : CourseManager
// =============================================================
const int max_courses=100;
class CourseManager{
private:
Course * pendingCourses[max_courses];
    int pendingCount;
 
    Course * approvedCourses[max_courses];
    int   approvedCount;
 
    Course * rejectedCourses[max_courses];
    int  rejectedCount;
 
    Course* findInAll(string courseID){     //This function searches for a course in all three lists — pending, approved, and rejected — and returns the matching course if found.
        for(int i=0;i<pendingCount;i++){
            if(pendingCourses[i]->getCourseID()==courseID){
                return pendingCourses[i];
            }
        }
         for(int i=0;i<approvedCount;i++){
            if(approvedCourses[i]->getCourseID()==courseID){
                return approvedCourses[i];
            }
        }
        for(int i=0;i<rejectedCount;i++){
            if(rejectedCourses[i]->getCourseID()==courseID){
                return rejectedCourses[i];
            }
        }   return nullptr;
    }

    int findPending(string courseID) const{
         for (int i = 0; i < pendingCount; i++)
            if (pendingCourses[i]->getCourseID()== courseID)
                return i;
        return -1;
    }
 public:
 CourseManager(){
    pendingCount=approvedCount=rejectedCount=0;
 }

 void submitCourse(Course *course){
    if(!course){
        throw invalid_argument("Course pointer is null");
    }
    string id= course->getCourseID();
    if(findInAll(id)){
        throw runtime_error("Course already exists with ID:" + id);
    
    }
    if(pendingCount>=max_courses){
        throw overflow_error(" Pending course limit is full");
    }                               // FIX: moved closing brace here; the two lines below were inside the throw block
    pendingCourses[pendingCount]=course;
    pendingCount++;
 }

 void approveCourse(string courseID){
 
    int index= findPending(courseID);
    if (index == -1)
    throw out_of_range("Course not found in pending list.");

 
 Course *course= pendingCourses[index];

 for(int i=index; i< pendingCount -1;i++){
    pendingCourses[i] = pendingCourses[i + 1];
 }                                  // FIX: closing brace for the for-loop was missing

        pendingCount--;

        course->setStatus("Approved");

        if (approvedCount >= max_courses)
            throw overflow_error("Approved course limit reached.");

        approvedCourses[approvedCount++] = course;
 }

 void rejectCourses(string courseID){
       int index= findPending(courseID);
    if (index == -1)
    throw out_of_range("Course not found in pending list.");

  Course* c = pendingCourses[index];

        for (int i = index; i < pendingCount - 1; i++)
            pendingCourses[i] = pendingCourses[i + 1];

        pendingCount--;

        c->setStatus("Rejected");

        if (rejectedCount >= max_courses)
            throw overflow_error("Rejected course limit reached.");

        rejectedCourses[rejectedCount++] = c;
 }
 void getPending(){
   cout<<"====Pending Courses====="<<endl;
   for(int i=0;i< pendingCount;i++){
       pendingCourses[i]->displayInfo();
   } 
 }
 void getApproved(){
    cout<<"====Approved Courses====="<<endl;
   for(int i=0;i< approvedCount;i++){
       approvedCourses[i]->displayInfo();
   } 
 }
 void getRejected(){
     cout<<"====Rejected Courses====="<<endl;
   for(int i=0;i< rejectedCount;i++){
       rejectedCourses[i]->displayInfo();
   } 
 }
 void printAll(){
    getApproved();
    getPending();
    getRejected();
 }
};


// =============================================================
// CLASS : AccountManager
// =============================================================
const int Max_User=100;
class AccountManager{
private:
User *userlist[Max_User];
int userCount;

    User* students[Max_User];
    int studentCount;

    User* teachers[Max_User];
    int teacherCount;

    int findUserIndex(string userID) const {
        for (int i = 0; i < userCount; i++) {
            if (userlist[i]->getUserID() == userID)   // FIX: getId() -> getUserID() (correct method name)
                return i;
        }
        return -1;
    }

    User* getOrThrow(string userID) const {
        int index = findUserIndex(userID);
        if (index == -1)
            throw out_of_range("No user with given ID");
        return userlist[index];
    }

    void removeFromArray(User* arr[], int &count, string userID) {
        for (int i = 0; i < count; i++) {
            if (arr[i]->getUserID() == userID) {   // FIX: getid() -> getUserID()
                for (int j = i; j < count - 1; j++)
                    arr[j] = arr[j + 1];
                count--;
                return;
            }
        }
    }

    public:
    AccountManager(){
        userCount=studentCount=teacherCount=0;
    }

    void addUser(User *user, const string &role) {   // FIX: USER* -> User*
        if (!user)
            throw invalid_argument("Null user pointer");

        string id = user->getUserID();   // FIX: getid() -> getUserID()

        if (findUserIndex(id) != -1)
            throw runtime_error("User already exists");

        if (userCount >= Max_User)
            throw overflow_error("User limit reached");
        userlist[userCount++] = user;

        if (role == "Student") {
            if (studentCount >= Max_User)
                throw overflow_error("Student list full");
            students[studentCount++] = user;
        }
        else if (role == "Teacher") {
            if (teacherCount >= Max_User)
                throw overflow_error("Teacher list full");
            teachers[teacherCount++] = user;
        }
    }

    void removeStudent(string studentID) {
        bool found = false;

        for (int i = 0; i < studentCount; i++) {
            if (students[i]->getUserID() == studentID) {   // FIX: getid() -> getUserID()
                found = true;
                break;
            }
        }

        if (!found) {
            if (findUserIndex(studentID) != -1)
                throw runtime_error("User is not a Student");
            throw out_of_range("Student not found");
        }

        removeFromArray(userlist, userCount, studentID);
        removeFromArray(students, studentCount, studentID);
    }

    void removeTeacher(string teacherID) {
        bool found = false;

        for (int i = 0; i < teacherCount; i++) {
            if (teachers[i]->getUserID() == teacherID) {   // FIX: getid() -> getUserID()
                found = true;
                break;
            }
        }

        if (!found) {
            if (findUserIndex(teacherID) != -1)
                throw runtime_error("User is not a Teacher");
            throw out_of_range("Teacher not found");
        }

        removeFromArray(userlist, userCount, teacherID);
        removeFromArray(teachers, teacherCount, teacherID);
    }

    void resetPassword(string userid, string newpass) {
        User *u = getOrThrow(userid);
        u->changePassword(newpass);   // FIX: setpass() -> changePassword() (correct method name)
    }

    void banUser(string userID) {
        User *u = getOrThrow(userID);
        // FIX: u->isbanned() and u->ban are not defined on User; kept as stub
        cout << "Ban functionality requires isbanned()/ban() on User.\n";
    }

 void printAll() const {
        cout << "\n--- All Users (" << userCount << ") ---\n";
        for (int i = 0; i < userCount; i++) {
            userlist[i]->viewProfile();   // FIX: print() -> viewProfile() (correct method name)
        }
    }
};


// =============================================================
// CLASS : Subscription
// =============================================================
const int MAX_FEATURES = 20;
const int MAX_STUDENTS_SUB = 200;   // FIX: renamed to avoid redefinition with GradeBook's MAX_STUDENTS
const int MAX_PLANS = 10;

class Subscription {
private:
    int planID;
    string planName;
    float price;
    int durationMonths;

    string features[MAX_FEATURES];
    int featureCount;

    int studentIDs[MAX_STUDENTS_SUB];
    string expiryDates[MAX_STUDENTS_SUB];
    int studentCount;

    static Subscription* catalogue[MAX_PLANS];
    static int catalogueCount;

    static string validatePlan(string name) {
        if (name != "Basic" && name != "Pro" && name != "Premium")
            throw invalid_argument("Invalid plan name");
        return name;
    }

    static string generateExpiry(int months) {
        return "After " + to_string(months) + " months";
    }

    int findStudent(int id) const {
        for (int i = 0; i < studentCount; i++)
            if (studentIDs[i] == id)
                return i;
        return -1;
    }

public:
    Subscription(int id, string name, float p, int duration, string feats[], int fCount) {
        if (p < 0)
            throw invalid_argument("Price cannot be negative");
        if (duration <= 0)
            throw invalid_argument("Duration must be positive");
        if (fCount <= 0)
            throw invalid_argument("Features required");

        planID = id;
        planName = validatePlan(name);
        price = p;
        durationMonths = duration;

        featureCount = fCount;
        for (int i = 0; i < fCount; i++)
            features[i] = feats[i];

        studentCount = 0;

        if (catalogueCount >= MAX_PLANS)
            throw overflow_error("Plan catalogue full");

        catalogue[catalogueCount++] = this;
    }

    void activate(int studentID) {
        if (findStudent(studentID) != -1)
            throw runtime_error("Student already subscribed");

        if (studentCount >= MAX_STUDENTS_SUB)
            throw overflow_error("Max students reached");

        studentIDs[studentCount] = studentID;
        expiryDates[studentCount] = generateExpiry(durationMonths);
        studentCount++;
    }

    void cancel(int studentID) {
        int index = findStudent(studentID);

        if (index == -1)
            throw runtime_error("Student not subscribed");

        for (int i = index; i < studentCount - 1; i++) {
            studentIDs[i] = studentIDs[i + 1];
            expiryDates[i] = expiryDates[i + 1];
        }

        studentCount--;
    }

    void print() const {
        cout << "Plan: " << planName << endl;
        cout << "Price: " << price << endl;
        cout << "Duration: " << durationMonths << " months\n";

        cout << "Features: ";
        for (int i = 0; i < featureCount; i++) {
            cout << features[i];
            if (i != featureCount - 1) cout << " | ";
        }

        cout << "\nSubscribers: " << studentCount << endl;
    }

    static void listPlans() {
        cout << "\n--- All Plans ---\n";
        for (int i = 0; i < catalogueCount; i++) {
            catalogue[i]->print();
            cout << "----------------\n";
        }
    }

    int getPlanID() const { return planID; }
    string getPlanName() const { return planName; }
    float getPrice() const { return price; }
};

Subscription* Subscription::catalogue[MAX_PLANS];
int Subscription::catalogueCount = 0;

