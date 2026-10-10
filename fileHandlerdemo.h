#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <string>

using namespace std;

// ============================================================
// CONSTANTS
// ============================================================
const int FH_MAX_USERS       = 200;
const int FH_MAX_COURSES     = 100;
const int FH_MAX_ENROLLMENTS = 500;
const int FH_MAX_GRADES      = 500;
const int FH_MAX_SUBMISSIONS = 300;
const int FH_MAX_TASKS       = 300;
const int FH_MAX_ZOOMLINKS   = 100;
const int FH_MAX_MATERIALS   = 200;
const int FH_MAX_ASSIGNMENTS = 200;

// ============================================================
// STRUCTS
// ============================================================

struct UserRecord {
    char  userID[20];
    char  name[60];
    char  email[60];
    char  password[60];
    char  role[20];
    int   isActive;
    char  registrationDate[20];
};

struct CourseRecord {
    char  courseID[20];
    char  title[100];
    char  description[200];
    float price;
    int   teacherID;
    int   prerequisiteID;
    int   isApproved;
    char  status[20];
    char  type[10];
    char  subscriptionPlan[20];
};

struct EnrollmentRecord {
    int  enrollmentID;
    int  studentID;
    char courseID[20];
    char enrolledDate[20];
    int  paymentStatus;
    int  isConfirmed;
};

struct GradeRecord {
    char  courseID[20];
    int   studentID;
    float grade;
};

struct SubmissionRecord {
    int   submissionID;
    int   studentID;
    int   assignmentID;
    char  filePath[100];
    char  submittedDate[20];
    float grade;
    int   isGraded;
};

struct TaskRecord {
    int  taskID;
    int  studentID;
    char title[100];
    char dueDate[20];
    char priority[10];
    int  isDone;
};

struct ZoomLinkRecord {
    int  linkID;
    char teacherName[60];
    char section[20];
    char courseTitle[100];
    char url[200];
    char scheduledTime[30];
};

struct MaterialRecord {
    int  materialID;
    char title[100];
    char fileType[20];
    char filePath[200];
    char uploadDate[20];
    int  courseID;
};

struct AssignmentRecord {
    int  assignmentID;
    char title[100];
    char description[200];
    char deadline[20];
    int  maxMarks;
    int  courseID;
};

// ============================================================
// CSV NAMESPACE (DECLARATION ONLY)
// ============================================================

namespace CSV {
    void copyStr(char* dest, int destSize, const string& src);
    string nextField(const string& line, int& pos);
    string escField(const char* s);
}

// ============================================================
// FileHandler CLASS (DECLARATION ONLY)
// ============================================================

class FileHandler {
private:
    char dataDir[50];

    void ensureDir() const;
    void buildPath(char* dest, int destSize, const char* filename) const;

public:
    FileHandler(const char* dir = "data/");

    // Users
    int saveUsers(UserRecord users[], int count);
    int loadUsers(UserRecord users[], int maxCount);
    void registerUser(const UserRecord& u);
    string checkLogin(const string& email, const string& password);
    bool emailExists(const string& email);
    bool changePassword(const string& email, const string& newPassword);

    // Courses
    int saveCourses(CourseRecord c[], int count);
    int loadCourses(CourseRecord c[], int maxCount);

    // Enrollments
    int saveEnrollments(EnrollmentRecord e[], int count);
    int loadEnrollments(EnrollmentRecord e[], int maxCount);

    // Grades
    int saveGrades(GradeRecord g[], int count);
    int loadGrades(GradeRecord g[], int maxCount);

    // Submissions
    int saveSubmissions(SubmissionRecord s[], int count);
    int loadSubmissions(SubmissionRecord s[], int maxCount);

    // Tasks
    int saveTasks(TaskRecord t[], int count);
    int loadTasks(TaskRecord t[], int maxCount);

    // Zoom
    int saveZoomLinks(ZoomLinkRecord z[], int count);
    int loadZoomLinks(ZoomLinkRecord z[], int maxCount);

    // Materials
    int saveMaterials(MaterialRecord m[], int count);
    int loadMaterials(MaterialRecord m[], int maxCount);

    // Assignments
    int saveAssignments(AssignmentRecord a[], int count);
    int loadAssignments(AssignmentRecord a[], int maxCount);

    // Activity log
    void logActivity(const char* actor, const char* action, const char* detail = "");
};

#endif