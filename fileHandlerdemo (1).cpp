#ifndef FILEHANDLER_CPP
#define FILEHANDLER_CPP

#include "fileHandlerdemo.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef _WIN32
#include <direct.h>
#endif

using namespace std;

// ============================================================
//  CSV HELPERS
// ============================================================
namespace CSV {

    void copyStr(char* dest, int destSize, const string& src) {
        int i = 0;
        for (; i < (int)src.size() && i < destSize - 1; i++)
            dest[i] = src[i];
        dest[i] = '\0';
    }

    string nextField(const string& line, int& pos) {
        string field;
        bool inQuotes = false;
        while (pos < (int)line.size()) {
            char c = line[pos];
            if (inQuotes) {
                if (c == '"' && pos + 1 < (int)line.size() && line[pos+1] == '"') {
                    field += '"'; pos += 2;
                } else if (c == '"') {
                    inQuotes = false; pos++;
                } else {
                    field += c; pos++;
                }
            } else {
                if      (c == '"') { inQuotes = true; pos++; }
                else if (c == ',') { pos++; break; }
                else               { field += c; pos++; }
            }
        }
        return field;
    }

    string escField(const char* s) {
        string out(s);
        bool needs = false;
        for (char c : out) if (c == ',' || c == '"') { needs = true; break; }
        if (!needs) return out;
        string q = "\"";
        for (char c : out) { if (c == '"') q += "\"\""; else q += c; }
        q += '"';
        return q;
    }

} // namespace CSV

// ============================================================
//  FileHandler IMPLEMENTATION
// ============================================================

void FileHandler::ensureDir() const {
#ifdef _WIN32
    _mkdir(dataDir);
#else
    mkdir(dataDir, 0755);
#endif
}

void FileHandler::buildPath(char* dest, int destSize, const char* filename) const {
    snprintf(dest, destSize, "%s%s", dataDir, filename);
}

FileHandler::FileHandler(const char* dir) {
    strncpy(dataDir, dir, 49);
    dataDir[49] = '\0';
    ensureDir();
}

// ---- USERS ----

void FileHandler::registerUser(const UserRecord& u) {
    char path[200];
    buildPath(path, 200, "users.csv");
    ofstream f(path, ios::app);
    if (!f) return;
    f << CSV::escField(u.userID) << ","
      << CSV::escField(u.name)   << ","
      << CSV::escField(u.email)  << ","
      << CSV::escField(u.password) << ","
      << CSV::escField(u.role)   << ","
      << u.isActive << ","
      << CSV::escField(u.registrationDate) << "\n";
}

int FileHandler::loadUsers(UserRecord users[], int maxCount) {
    char path[200];
    buildPath(path, 200, "users.csv");
    ifstream f(path);
    if (!f) return 0;
    int count = 0;
    string line;
    while (getline(f, line) && count < maxCount) {
        if (line.empty()) continue;
        int pos = 0;
        CSV::copyStr(users[count].userID,   20, CSV::nextField(line, pos));
        CSV::copyStr(users[count].name,     60, CSV::nextField(line, pos));
        CSV::copyStr(users[count].email,    60, CSV::nextField(line, pos));
        CSV::copyStr(users[count].password, 60, CSV::nextField(line, pos));
        CSV::copyStr(users[count].role,     20, CSV::nextField(line, pos));
        users[count].isActive = stoi(CSV::nextField(line, pos));
        CSV::copyStr(users[count].registrationDate, 20, CSV::nextField(line, pos));
        count++;
    }
    return count;
}

int FileHandler::saveUsers(UserRecord users[], int count) {
    char path[200];
    buildPath(path, 200, "users.csv");
    ofstream f(path);
    if (!f) return 0;
    for (int i = 0; i < count; i++) {
        f << CSV::escField(users[i].userID)   << ","
          << CSV::escField(users[i].name)     << ","
          << CSV::escField(users[i].email)    << ","
          << CSV::escField(users[i].password) << ","
          << CSV::escField(users[i].role)     << ","
          << users[i].isActive << ","
          << CSV::escField(users[i].registrationDate) << "\n";
    }
    return count;
}

string FileHandler::checkLogin(const string& email, const string& password) {
    UserRecord users[FH_MAX_USERS];
    int count = loadUsers(users, FH_MAX_USERS);
    for (int i = 0; i < count; i++) {
        if (string(users[i].email) == email && string(users[i].password) == password)
            return string(users[i].role);
    }
    return "";
}

bool FileHandler::emailExists(const string& email) {
    UserRecord users[FH_MAX_USERS];
    int count = loadUsers(users, FH_MAX_USERS);
    for (int i = 0; i < count; i++)
        if (string(users[i].email) == email) return true;
    return false;
}

bool FileHandler::changePassword(const string& email, const string& newPassword) {
    UserRecord users[FH_MAX_USERS];
    int count = loadUsers(users, FH_MAX_USERS);
    bool found = false;
    for (int i = 0; i < count; i++) {
        if (string(users[i].email) == email) {
            CSV::copyStr(users[i].password, 60, newPassword);
            found = true;
            break;
        }
    }
    if (found) saveUsers(users, count);
    return found;
}

// ---- COURSES ----

int FileHandler::saveCourses(CourseRecord c[], int count) {
    char path[200];
    buildPath(path, 200, "courses.csv");
    ofstream f(path);
    if (!f) return 0;
    for (int i = 0; i < count; i++) {
        f << CSV::escField(c[i].courseID)       << ","
          << CSV::escField(c[i].title)          << ","
          << CSV::escField(c[i].description)    << ","
          << c[i].price     << ","
          << c[i].teacherID << ","
          << c[i].prerequisiteID << ","
          << c[i].isApproved << ","
          << CSV::escField(c[i].status)         << ","
          << CSV::escField(c[i].type)           << ","
          << CSV::escField(c[i].subscriptionPlan) << "\n";
    }
    return count;
}

int FileHandler::loadCourses(CourseRecord c[], int maxCount) {
    char path[200];
    buildPath(path, 200, "courses.csv");
    ifstream f(path);
    if (!f) return 0;
    int count = 0;
    string line;
    while (getline(f, line) && count < maxCount) {
        if (line.empty()) continue;
        int pos = 0;
        CSV::copyStr(c[count].courseID,    20,  CSV::nextField(line, pos));
        CSV::copyStr(c[count].title,       100, CSV::nextField(line, pos));
        CSV::copyStr(c[count].description, 200, CSV::nextField(line, pos));
        c[count].price         = stof(CSV::nextField(line, pos));
        c[count].teacherID     = stoi(CSV::nextField(line, pos));
        c[count].prerequisiteID= stoi(CSV::nextField(line, pos));
        c[count].isApproved    = stoi(CSV::nextField(line, pos));
        CSV::copyStr(c[count].status,           20, CSV::nextField(line, pos));
        CSV::copyStr(c[count].type,             10, CSV::nextField(line, pos));
        CSV::copyStr(c[count].subscriptionPlan, 20, CSV::nextField(line, pos));
        count++;
    }
    return count;
}

// ---- ENROLLMENTS ----

int FileHandler::saveEnrollments(EnrollmentRecord e[], int count) {
    char path[200];
    buildPath(path, 200, "enrollments.csv");
    ofstream f(path);
    if (!f) return 0;
    for (int i = 0; i < count; i++) {
        f << e[i].enrollmentID << ","
          << e[i].studentID   << ","
          << CSV::escField(e[i].courseID) << ","
          << CSV::escField(e[i].enrolledDate) << ","
          << e[i].paymentStatus << ","
          << e[i].isConfirmed  << "\n";
    }
    return count;
}

int FileHandler::loadEnrollments(EnrollmentRecord e[], int maxCount) {
    char path[200];
    buildPath(path, 200, "enrollments.csv");
    ifstream f(path);
    if (!f) return 0;
    int count = 0;
    string line;
    while (getline(f, line) && count < maxCount) {
        if (line.empty()) continue;
        int pos = 0;
        e[count].enrollmentID = stoi(CSV::nextField(line, pos));
        e[count].studentID    = stoi(CSV::nextField(line, pos));
        CSV::copyStr(e[count].courseID,     20, CSV::nextField(line, pos));
        CSV::copyStr(e[count].enrolledDate, 20, CSV::nextField(line, pos));
        e[count].paymentStatus = stoi(CSV::nextField(line, pos));
        e[count].isConfirmed   = stoi(CSV::nextField(line, pos));
        count++;
    }
    return count;
}

// ---- GRADES ----

int FileHandler::saveGrades(GradeRecord g[], int count) {
    char path[200];
    buildPath(path, 200, "grades.csv");
    ofstream f(path);
    if (!f) return 0;
    for (int i = 0; i < count; i++) {
        f << CSV::escField(g[i].courseID) << ","
          << g[i].studentID << ","
          << g[i].grade     << "\n";
    }
    return count;
}

int FileHandler::loadGrades(GradeRecord g[], int maxCount) {
    char path[200];
    buildPath(path, 200, "grades.csv");
    ifstream f(path);
    if (!f) return 0;
    int count = 0;
    string line;
    while (getline(f, line) && count < maxCount) {
        if (line.empty()) continue;
        int pos = 0;
        CSV::copyStr(g[count].courseID, 20, CSV::nextField(line, pos));
        g[count].studentID = stoi(CSV::nextField(line, pos));
        g[count].grade     = stof(CSV::nextField(line, pos));
        count++;
    }
    return count;
}

// ---- SUBMISSIONS ----

int FileHandler::saveSubmissions(SubmissionRecord s[], int count) {
    char path[200];
    buildPath(path, 200, "submissions.csv");
    ofstream f(path);
    if (!f) return 0;
    for (int i = 0; i < count; i++) {
        f << s[i].submissionID << ","
          << s[i].studentID   << ","
          << s[i].assignmentID << ","
          << CSV::escField(s[i].filePath)      << ","
          << CSV::escField(s[i].submittedDate) << ","
          << s[i].grade     << ","
          << s[i].isGraded  << "\n";
    }
    return count;
}

int FileHandler::loadSubmissions(SubmissionRecord s[], int maxCount) {
    char path[200];
    buildPath(path, 200, "submissions.csv");
    ifstream f(path);
    if (!f) return 0;
    int count = 0;
    string line;
    while (getline(f, line) && count < maxCount) {
        if (line.empty()) continue;
        int pos = 0;
        s[count].submissionID  = stoi(CSV::nextField(line, pos));
        s[count].studentID     = stoi(CSV::nextField(line, pos));
        s[count].assignmentID  = stoi(CSV::nextField(line, pos));
        CSV::copyStr(s[count].filePath,      100, CSV::nextField(line, pos));
        CSV::copyStr(s[count].submittedDate, 20,  CSV::nextField(line, pos));
        s[count].grade    = stof(CSV::nextField(line, pos));
        s[count].isGraded = stoi(CSV::nextField(line, pos));
        count++;
    }
    return count;
}

// ---- TASKS ----

int FileHandler::saveTasks(TaskRecord t[], int count) {
    char path[200];
    buildPath(path, 200, "tasks.csv");
    ofstream f(path);
    if (!f) return 0;
    for (int i = 0; i < count; i++) {
        f << t[i].taskID    << ","
          << t[i].studentID << ","
          << CSV::escField(t[i].title)    << ","
          << CSV::escField(t[i].dueDate)  << ","
          << CSV::escField(t[i].priority) << ","
          << t[i].isDone << "\n";
    }
    return count;
}

int FileHandler::loadTasks(TaskRecord t[], int maxCount) {
    char path[200];
    buildPath(path, 200, "tasks.csv");
    ifstream f(path);
    if (!f) return 0;
    int count = 0;
    string line;
    while (getline(f, line) && count < maxCount) {
        if (line.empty()) continue;
        int pos = 0;
        t[count].taskID    = stoi(CSV::nextField(line, pos));
        t[count].studentID = stoi(CSV::nextField(line, pos));
        CSV::copyStr(t[count].title,    100, CSV::nextField(line, pos));
        CSV::copyStr(t[count].dueDate,   20, CSV::nextField(line, pos));
        CSV::copyStr(t[count].priority,  10, CSV::nextField(line, pos));
        t[count].isDone = stoi(CSV::nextField(line, pos));
        count++;
    }
    return count;
}

// ---- ZOOM LINKS ----

int FileHandler::saveZoomLinks(ZoomLinkRecord z[], int count) {
    char path[200];
    buildPath(path, 200, "zoom.csv");
    ofstream f(path);
    if (!f) return 0;
    for (int i = 0; i < count; i++) {
        f << z[i].linkID << ","
          << CSV::escField(z[i].teacherName)   << ","
          << CSV::escField(z[i].section)       << ","
          << CSV::escField(z[i].courseTitle)   << ","
          << CSV::escField(z[i].url)           << ","
          << CSV::escField(z[i].scheduledTime) << "\n";
    }
    return count;
}

int FileHandler::loadZoomLinks(ZoomLinkRecord z[], int maxCount) {
    char path[200];
    buildPath(path, 200, "zoom.csv");
    ifstream f(path);
    if (!f) return 0;
    int count = 0;
    string line;
    while (getline(f, line) && count < maxCount) {
        if (line.empty()) continue;
        int pos = 0;
        z[count].linkID = stoi(CSV::nextField(line, pos));
        CSV::copyStr(z[count].teacherName,   60,  CSV::nextField(line, pos));
        CSV::copyStr(z[count].section,       20,  CSV::nextField(line, pos));
        CSV::copyStr(z[count].courseTitle,   100, CSV::nextField(line, pos));
        CSV::copyStr(z[count].url,           200, CSV::nextField(line, pos));
        CSV::copyStr(z[count].scheduledTime, 30,  CSV::nextField(line, pos));
        count++;
    }
    return count;
}

// ---- MATERIALS ----

int FileHandler::saveMaterials(MaterialRecord m[], int count) {
    char path[200];
    buildPath(path, 200, "materials.csv");
    ofstream f(path);
    if (!f) return 0;
    for (int i = 0; i < count; i++) {
        f << m[i].materialID << ","
          << CSV::escField(m[i].title)      << ","
          << CSV::escField(m[i].fileType)   << ","
          << CSV::escField(m[i].filePath)   << ","
          << CSV::escField(m[i].uploadDate) << ","
          << m[i].courseID << "\n";
    }
    return count;
}

int FileHandler::loadMaterials(MaterialRecord m[], int maxCount) {
    char path[200];
    buildPath(path, 200, "materials.csv");
    ifstream f(path);
    if (!f) return 0;
    int count = 0;
    string line;
    while (getline(f, line) && count < maxCount) {
        if (line.empty()) continue;
        int pos = 0;
        m[count].materialID = stoi(CSV::nextField(line, pos));
        CSV::copyStr(m[count].title,      100, CSV::nextField(line, pos));
        CSV::copyStr(m[count].fileType,    20, CSV::nextField(line, pos));
        CSV::copyStr(m[count].filePath,   200, CSV::nextField(line, pos));
        CSV::copyStr(m[count].uploadDate,  20, CSV::nextField(line, pos));
        m[count].courseID = stoi(CSV::nextField(line, pos));
        count++;
    }
    return count;
}

// ---- ASSIGNMENTS ----

int FileHandler::saveAssignments(AssignmentRecord a[], int count) {
    char path[200];
    buildPath(path, 200, "assignments.csv");
    ofstream f(path);
    if (!f) return 0;
    for (int i = 0; i < count; i++) {
        f << a[i].assignmentID << ","
          << CSV::escField(a[i].title)       << ","
          << CSV::escField(a[i].description) << ","
          << CSV::escField(a[i].deadline)    << ","
          << a[i].maxMarks  << ","
          << a[i].courseID  << "\n";
    }
    return count;
}

int FileHandler::loadAssignments(AssignmentRecord a[], int maxCount) {
    char path[200];
    buildPath(path, 200, "assignments.csv");
    ifstream f(path);
    if (!f) return 0;
    int count = 0;
    string line;
    while (getline(f, line) && count < maxCount) {
        if (line.empty()) continue;
        int pos = 0;
        a[count].assignmentID = stoi(CSV::nextField(line, pos));
        CSV::copyStr(a[count].title,       100, CSV::nextField(line, pos));
        CSV::copyStr(a[count].description, 200, CSV::nextField(line, pos));
        CSV::copyStr(a[count].deadline,     20, CSV::nextField(line, pos));
        a[count].maxMarks = stoi(CSV::nextField(line, pos));
        a[count].courseID = stoi(CSV::nextField(line, pos));
        count++;
    }
    return count;
}

// ---- ACTIVITY LOG ----

void FileHandler::logActivity(const char* actor, const char* action, const char* detail) {
    char path[200];
    buildPath(path, 200, "activity_log.csv");
    ofstream f(path, ios::app);
    if (!f) return;
    f << CSV::escField(actor)  << ","
      << CSV::escField(action) << ","
      << CSV::escField(detail) << "\n";
}

#endif