#pragma once
// ============================================================
//  gui.h  —  Shared declarations for SAM E-Learning Platform
//  Include this in main.cpp ONLY (not in FileHandler.cpp)
// ============================================================

#include "raylib.h"
#include "fileHandlerdemo.h"   // structs + FileHandler class
#include <string>
#include <cstring>
using namespace std;

// ============================================================
//  SCREEN IDs
// ============================================================
enum Screen {
    SCR_HOME = 0,
    SCR_LOGIN,
    SCR_REGISTER_STUDENT,
    SCR_REGISTER_TEACHER,
    SCR_AFTER_REGISTER,
    SCR_STUDENT_DASHBOARD,
    SCR_TEACHER_DASHBOARD,
    SCR_ADMIN_DASHBOARD,
    SCR_GRADEBOOK,
    SCR_COURSES,
    SCR_TASKS,
    SCR_ZOOM,
    SCR_QUIZ,
    SCR_TEACHER_COURSES,
    SCR_UPLOAD_MATERIAL,
    SCR_UPLOAD_QUIZ,
    SCR_VIEW_SUBMISSIONS,
    SCR_TEACHER_GRADEBOOK,
    SCR_ADMIN_APPROVE_COURSES,
    SCR_ADMIN_APPROVE_REGS,
    SCR_ADMIN_MANAGE_USERS,
    SCR_ADMIN_SUBSCRIPTIONS,
    SCR_ADMIN_ENROLLMENTS
};

// ============================================================
//  WINDOW SIZE
// ============================================================
static const int SW = 1920;
static const int SH = 1080;

// ============================================================
//  THEME COLORS  (matching the pink/purple/teal screenshot)
// ============================================================
static const Color COL_BG          = {245, 240, 255, 255};   // soft lavender bg
static const Color COL_TOPBAR      = {40,  36,  60,  255};   // dark purple top bar
static const Color COL_PINK        = {255, 182, 213, 255};   // hero pink
static const Color COL_CARD_AUTH   = {180, 170, 240, 255};   // purple card
static const Color COL_CARD_TEACH  = {120, 210, 180, 255};   // teal card
static const Color COL_CARD_ADMIN1 = {255, 200, 100, 255};   // yellow card
static const Color COL_CARD_ADMIN2 = {100, 200, 255, 255};   // blue card
static const Color COL_ACCENT      = {255, 100, 150, 255};   // hot pink accent
static const Color COL_BTN_GREEN   = {60,  180, 120, 255};
static const Color COL_BTN_RED     = {220,  70,  70, 255};
static const Color COL_BTN_BLUE    = {60,  120, 220, 255};
static const Color COL_BTN_PURPLE  = {130,  80, 200, 255};
static const Color COL_TEXT_DARK   = {30,   25,  50, 255};
static const Color COL_TEXT_LIGHT  = {255, 255, 255, 255};
static const Color COL_INPUT_BG    = {255, 255, 255, 255};
static const Color COL_INPUT_BD    = {180, 160, 220, 255};
static const Color COL_ROW_ALT     = {235, 228, 255, 255};

// ============================================================
//  GLOBALS  (defined in gui.cpp, used everywhere)
// ============================================================
extern Screen        currentScreen;
extern string        loggedEmail;
extern string        loggedRole;
extern string        loggedName;
extern FileHandler*  fh;
extern float         scrollY;
extern float         feedbackTimer;
extern string        feedbackMsg;
extern bool          feedbackOK;
extern int           inputFocus;

// shared text input buffers
extern char inp0[128], inp1[128], inp2[128], inp3[128], inp4[128];

// cached data arrays
extern UserRecord       gUsers[];
extern int              gUserCount;
extern CourseRecord     gCourses[];
extern int              gCourseCount;
extern GradeRecord      gGrades[];
extern int              gGradeCount;
extern EnrollmentRecord gEnrollments[];
extern int              gEnrollCount;
extern TaskRecord       gTasks[];
extern int              gTaskCount;
extern ZoomLinkRecord   gZoom[];
extern int              gZoomCount;
extern SubmissionRecord gSubs[];
extern int              gSubCount;
extern AssignmentRecord gAssigns[];
extern int              gAssignCount;

// ============================================================
//  HELPER FUNCTION DECLARATIONS
// ============================================================
void   reloadAll();
void   clearInputs();
void   setFeedback(const string& msg, bool ok);
bool   DrawButton(const char* label, Rectangle r, Color bg, Color fg, int fs);
void   DrawInputBox(const char* label, Rectangle r, char* buf, int bufSize, int idx, bool password=false);
void   DrawPanel(Rectangle r, Color bg, float radius=10.f);
void   DrawFeedback();
void   DrawTopBar();

// ============================================================
//  SCREEN DRAW DECLARATIONS
// ============================================================
void DrawHome();
void DrawLogin();
void DrawRegister(bool isTeacher);
void DrawAfterRegister();
void DrawStudentDashboard();
void DrawTeacherDashboard();
void DrawAdminDashboard();
void DrawGradebook();
void DrawMyCourses();
void DrawTasks();
void DrawZoomLinks();
void DrawQuiz();
void DrawTeacherCourses();
void DrawUploadMaterial();
void DrawUploadQuiz();
void DrawViewSubmissions();
void DrawTeacherGradebook();
void DrawAdminApproveCourses();
void DrawAdminApproveRegs();
void DrawAdminManageUsers();
void DrawAdminSubscriptions();
void DrawAdminEnrollments();