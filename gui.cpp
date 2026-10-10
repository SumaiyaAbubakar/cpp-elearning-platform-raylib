// ============================================================
//  gui.cpp  —  Full Raylib GUI for SAM E-Learning Platform
//
//  DO NOT repeat FileHandler or OOP class code here.
//  This file includes gui.h which includes FileHandler.cpp.
//  main.cpp must: #include "gui.h"  and remove its own main().
//
//  Compile command (Git Bash / PowerShell):
//    g++ -std=c++17 gui.cpp -o SAM -I"C:/raylib/include" \
//        -L"C:/raylib/lib" -lraylib -lopengl32 -lgdi32 -lwinmm
//  Adjust paths to wherever your raylib is installed.
//  Run:  ./SAM
// ============================================================

#include "gui.h"
#include <cstdio>
#include <cstring>
#include <string>
using namespace std;

// ============================================================
//  GLOBAL DEFINITIONS
// ============================================================
Screen        currentScreen = SCR_HOME;
string        loggedEmail   = "";
string        loggedRole    = "";
string        loggedName    = "";
FileHandler*  fh            = nullptr;
float         scrollY       = 0.f;
float         feedbackTimer = 0.f;
string        feedbackMsg   = "";
bool          feedbackOK    = true;
int           inputFocus    = -1;

char inp0[128]={}, inp1[128]={}, inp2[128]={}, inp3[128]={}, inp4[128]={};

UserRecord       gUsers[FH_MAX_USERS];       int gUserCount=0;
CourseRecord     gCourses[FH_MAX_COURSES];   int gCourseCount=0;
GradeRecord      gGrades[FH_MAX_GRADES];     int gGradeCount=0;
EnrollmentRecord gEnrollments[FH_MAX_ENROLLMENTS]; int gEnrollCount=0;
TaskRecord       gTasks[FH_MAX_TASKS];       int gTaskCount=0;
ZoomLinkRecord   gZoom[FH_MAX_ZOOMLINKS];    int gZoomCount=0;
SubmissionRecord gSubs[FH_MAX_SUBMISSIONS];  int gSubCount=0;
AssignmentRecord gAssigns[FH_MAX_ASSIGNMENTS]; int gAssignCount=0;

// ============================================================
//  HELPERS
// ============================================================
void reloadAll(){
    gUserCount    = fh->loadUsers(gUsers, FH_MAX_USERS);
    gCourseCount  = fh->loadCourses(gCourses, FH_MAX_COURSES);
    gGradeCount   = fh->loadGrades(gGrades, FH_MAX_GRADES);
    gEnrollCount  = fh->loadEnrollments(gEnrollments, FH_MAX_ENROLLMENTS);
    gTaskCount    = fh->loadTasks(gTasks, FH_MAX_TASKS);
    gZoomCount    = fh->loadZoomLinks(gZoom, FH_MAX_ZOOMLINKS);
    gSubCount     = fh->loadSubmissions(gSubs, FH_MAX_SUBMISSIONS);
    gAssignCount  = fh->loadAssignments(gAssigns, FH_MAX_ASSIGNMENTS);
}

void clearInputs(){
    memset(inp0,0,128); memset(inp1,0,128); memset(inp2,0,128);
    memset(inp3,0,128); memset(inp4,0,128);
    inputFocus=-1;
}

void setFeedback(const string& msg, bool ok){
    feedbackMsg=msg; feedbackOK=ok; feedbackTimer=3.0f;
}

// Rounded-rect button — returns true if clicked
bool DrawButton(const char* label, Rectangle r, Color bg, Color fg, int fs){
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, r);
    Color draw = hover ? Color{(unsigned char)min(255,(int)bg.r+30),
                               (unsigned char)min(255,(int)bg.g+30),
                               (unsigned char)min(255,(int)bg.b+30),255} : bg;
    DrawRectangleRounded(r, 0.35f, 8, draw);
    int tw = MeasureText(label, fs);
    DrawText(label, (int)(r.x + r.width/2 - tw/2),
             (int)(r.y + r.height/2 - fs/2), fs, fg);
    return hover && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
}

// Single-line text input box
void DrawInputBox(const char* label, Rectangle r, char* buf, int bufSize, int idx, bool password){
    // label above
    DrawText(label, (int)r.x, (int)r.y - 18, 14, COL_TEXT_DARK);
    bool focused = (inputFocus == idx);
    Color bd = focused ? COL_ACCENT : COL_INPUT_BD;
    DrawRectangleRounded(r, 0.25f, 6, COL_INPUT_BG);
    DrawRectangleRoundedLines(r, 0.25f, 6, 2.f, bd);

    if(focused && IsMouseButtonReleased(MOUSE_LEFT_BUTTON) &&
       !CheckCollisionPointRec(GetMousePosition(), r))
        inputFocus = -1;
    if(CheckCollisionPointRec(GetMousePosition(), r) &&
       IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        inputFocus = idx;

    if(focused){
        int key = GetCharPressed();
        while(key > 0){
            int len = strlen(buf);
            if(key >= 32 && len < bufSize-1){
                buf[len]   = (char)key;
                buf[len+1] = '\0';
            }
            key = GetCharPressed();
        }
        if(IsKeyPressed(KEY_BACKSPACE)){
            int len = strlen(buf);
            if(len > 0) buf[len-1] = '\0';
        }
    }

    // display text
    int len = strlen(buf);
    string disp = "";
    if(password) for(int i=0;i<len;i++) disp += '*';
    else         disp = string(buf);
    string show = disp.size() > 30 ? disp.substr(disp.size()-30) : disp;
    DrawText(show.c_str(), (int)r.x+8, (int)r.y+r.height/2-8, 15, COL_TEXT_DARK);
    if(focused && ((int)(GetTime()*2) % 2 == 0))
        DrawText("|", (int)r.x+10+MeasureText(show.c_str(),15),
                 (int)r.y+r.height/2-8, 15, COL_TEXT_DARK);
}

void DrawPanel(Rectangle r, Color bg, float radius){
    DrawRectangleRounded(r, radius/100.f, 8, bg);
}

void DrawFeedback(){
    if(feedbackTimer <= 0) return;
    Color bg = feedbackOK ? Color{50,180,100,230} : Color{220,60,60,230};
    int tw = MeasureText(feedbackMsg.c_str(), 16);
    Rectangle r = {(float)(SW/2 - tw/2 - 20), (float)(SH-70), (float)(tw+40), 36};
    DrawRectangleRounded(r, 0.4f, 6, bg);
    DrawText(feedbackMsg.c_str(), (int)r.x+20, (int)r.y+10, 16, WHITE);
}

// Scrollable area helper: call before drawing list items
static float listScroll = 0;
void HandleScroll(float& scroll, float maxScroll){
    float wheel = GetMouseWheelMove();
    scroll -= wheel * 30.f;
    if(scroll < 0) scroll = 0;
    if(scroll > maxScroll) scroll = maxScroll;
}

// ============================================================
//  HOME SCREEN
// ============================================================
void DrawHome(){
    // Hero banner (pink)
    DrawRectangleRounded({10,62,SW-20,200}, 0.05f, 8, COL_PINK);
    DrawText("E-Learning Platform", 40, 100, 48, COL_TEXT_DARK);
    DrawText("Welcome to SAM - your smart academic manager.", 40, 148, 22, COL_TEXT_DARK);
    DrawText("Login or register to get started.", 40, 175, 20, {80,60,100,255});

    // Quick-access cards row — 3 cards evenly spaced
    int cardY = 285, cardH = 280;
    int gap   = 15;
    int cardW = (SW - 20 - 2*gap) / 3;  // 3 equal cards with 2 gaps between them

    int x0 = 10;
    int x1 = x0 + cardW + gap;
    int x2 = x1 + cardW + gap;

    // ── Auth card (purple) ──
    DrawRectangleRounded({(float)x0,(float)cardY,(float)cardW,(float)cardH},0.1f,8,COL_CARD_AUTH);
    DrawText("Auth Screens",  x0+8, cardY+12, 18, WHITE);
    DrawText("Authorization", x0+8, cardY+30, 15, {220,210,255,255});
    const char* authLabels[] = {"Login Screen","Register (Student)","Register (Teacher)","After Register","My Tasks"};
   Screen  authTargets[]= {SCR_LOGIN, SCR_REGISTER_STUDENT, SCR_REGISTER_TEACHER, SCR_AFTER_REGISTER, SCR_TASKS};
    for(int i=0;i<5;i++){
    if(DrawButton(authLabels[i],{(float)(x0+8),(float)(cardY+55+i*40),(float)(cardW-16),32},
                  {100,90,200,255},WHITE,16))
        currentScreen=authTargets[i];
}
    // ── Teacher card (green) ──
    DrawRectangleRounded({(float)x1,(float)cardY,(float)cardW,(float)cardH},0.1f,8,COL_CARD_TEACH);
    DrawText("Teacher Tabs",  x1+10, cardY+12, 18, WHITE);
    DrawText("Teacher Panel", x1+10, cardY+30, 15, {200,240,230,255});
    const char* teachLabels[] = {"My Courses","Upload Materials","Upload Quiz","View Submissions","Gradebook"};
    Screen      teachTargets[]= {SCR_TEACHER_COURSES, SCR_UPLOAD_MATERIAL, SCR_UPLOAD_QUIZ, SCR_VIEW_SUBMISSIONS, SCR_TEACHER_GRADEBOOK};
   for(int i=0;i<5;i++){
    if(DrawButton(teachLabels[i],{(float)(x1+10),(float)(cardY+55+i*40),(float)(cardW-16),32},
                  {60,160,130,255},WHITE,16))
        currentScreen=teachTargets[i];
}

    // ── Admin card (blue only) ──
    // ── Admin card (blue only) ──
int adminCardH = 310;
DrawRectangleRounded({(float)x2,(float)cardY,(float)cardW,(float)adminCardH},0.1f,8,COL_CARD_ADMIN2);
DrawText("Admin Tabs",    x2+10, cardY+12, 18, COL_TEXT_DARK);
DrawText("Admin Control", x2+10, cardY+30, 15, {0,80,140,255});
const char* adm1Labels[] = {"Approve Courses","Approve Regs","Manage Users","Subscriptions","Zoom Links","Monitor Enroll"};
Screen      adm1Targets[]= {SCR_ADMIN_APPROVE_COURSES, SCR_ADMIN_APPROVE_REGS, SCR_ADMIN_MANAGE_USERS, SCR_ADMIN_SUBSCRIPTIONS, SCR_ZOOM, SCR_ADMIN_ENROLLMENTS};
for(int i=0;i<6;i++){
    if(DrawButton(adm1Labels[i],{(float)(x2+10),(float)(cardY+50+i*40),(float)(cardW-16),32},
                  {40,140,200,255},WHITE,16))
        currentScreen=adm1Targets[i];
}

    // ── Bottom CTA buttons ──
    if(DrawButton("LOGIN",{(float)(SW/2-130),610,120,44},COL_BTN_PURPLE,WHITE,18))
    currentScreen=SCR_LOGIN;
if(DrawButton("REGISTER",{(float)(SW/2+10),610,120,44},COL_BTN_GREEN,WHITE,18))
    currentScreen=SCR_REGISTER_STUDENT;
}

// ============================================================
//  LOGIN
// ============================================================

// static int selectedRole = 0;  // 0=Student, 1=Teacher, 2=Admin

static int selectedRole = 0;

void DrawLogin(){
    DrawPanel({(float)(SW/2-220),80,440,420},WHITE,15.f);
    DrawText("Login to SAM", SW/2-90, 100, 30, COL_BTN_PURPLE);

    // ── Role selector ──
    DrawText("Login as:", SW/2-180, 148, 14, COL_TEXT_DARK);
    const char* roles[] = {"Student", "Teacher", "Admin"};
    Color roleCols[] = {COL_BTN_PURPLE, COL_CARD_TEACH, COL_CARD_ADMIN1};
    for(int i = 0; i < 3; i++){
        Color bg = (selectedRole == i) ? roleCols[i] : Color{200,195,210,255};
        if(DrawButton(roles[i], {(float)(SW/2-180 + i*125), 168, 110, 32}, bg, WHITE, 14))
            selectedRole = i;
    }

    DrawInputBox("Email",    {(float)(SW/2-180), 228, 360, 38}, inp0, 128, 0);
    DrawInputBox("Password", {(float)(SW/2-180), 300, 360, 38}, inp1, 128, 1, true);

    if(DrawButton("LOGIN", {(float)(SW/2-90), 360, 180, 42}, COL_BTN_PURPLE, WHITE, 17)){
        string role = fh->checkLogin(string(inp0), string(inp1));
        if(role.empty()){
            setFeedback("Invalid email or password!", false);
        } else {
            // ── THE ACTUAL FIX: verify role matches selection ──
            string expectedRole = roles[selectedRole];
            if(role != expectedRole){
                setFeedback("Wrong role selected for this account!", false);
            } else {
                loggedEmail = string(inp0);
                loggedRole  = role;
                for(int i = 0; i < gUserCount; i++)
                    if(string(gUsers[i].email) == loggedEmail)
                        { loggedName = string(gUsers[i].name); break; }
                clearInputs();
                if(role == "Student")      currentScreen = SCR_STUDENT_DASHBOARD;
                else if(role == "Teacher") currentScreen = SCR_TEACHER_DASHBOARD;
                else                       currentScreen = SCR_ADMIN_DASHBOARD;
                setFeedback("Welcome, " + loggedName + "!", true);
            }
        }
    }

    DrawText("No account?", SW/2-85, 415, 20, COL_TEXT_DARK);
    if(DrawButton("Register as Student",{(float)(SW/2-180),435,190,34},COL_BTN_GREEN,WHITE,13))
        { clearInputs(); currentScreen=SCR_REGISTER_STUDENT; }
    if(DrawButton("Register as Teacher",{(float)(SW/2+10),435,190,34},COL_CARD_TEACH,WHITE,13))
        { clearInputs(); currentScreen=SCR_REGISTER_TEACHER; }
}

// ============================================================
//  REGISTER
// ============================================================
void DrawRegister(bool isTeacher){
    const char* title = isTeacher ? "Register as Teacher" : "Register as Student";
    DrawPanel({(float)(SW/2-240),70,480,430},WHITE,15.f);
    DrawText(title, SW/2 - MeasureText(title,28)/2, 90, 28, isTeacher ? COL_CARD_TEACH : COL_BTN_PURPLE);

    DrawInputBox("Full Name",{(float)(SW/2-200),148,400,38},inp0,128,0);
    DrawInputBox("Email",    {(float)(SW/2-200),220,400,38},inp1,128,1);
    DrawInputBox("Password", {(float)(SW/2-200),292,400,38},inp2,128,2,true);
    DrawInputBox("User ID (e.g. STU_002)",{(float)(SW/2-200),364,400,38},inp3,128,3);

    Color btnC = isTeacher ? COL_CARD_TEACH : COL_BTN_PURPLE;
    if(DrawButton("REGISTER",{(float)(SW/2-90),420,180,42},btnC,WHITE,17)){
        if(strlen(inp0)<2||strlen(inp1)<3||strlen(inp2)<4||strlen(inp3)<2){
            setFeedback("Please fill all fields!",false);
        } else if(fh->emailExists(string(inp1))){
            setFeedback("Email already registered!",false);
        } else {
            UserRecord u; memset(&u,0,sizeof(u));
            strncpy(u.userID,inp3,19);
            strncpy(u.name,inp0,59);
            strncpy(u.email,inp1,59);
            strncpy(u.password,inp2,59);
            strncpy(u.role, isTeacher?"Teacher":"Student",19);
            u.isActive=1;
            strncpy(u.registrationDate,"2025-01-01",19);
            fh->registerUser(u);
            fh->logActivity(u.name, "Register", isTeacher?"Teacher":"Student");
            reloadAll();
            clearInputs();
            setFeedback("Registration successful! Await admin approval.",true);
            currentScreen=SCR_AFTER_REGISTER;
        }
    }
    if(DrawButton("Back to Login",{(float)(SW/2-60),475,120,28},COL_BTN_RED,WHITE,13))
        { clearInputs(); currentScreen=SCR_LOGIN; }
}

// ============================================================
//  AFTER REGISTER
// ============================================================
void DrawAfterRegister(){
    DrawPanel({(float)(SW/2-240),120,480,260},WHITE,15.f);
    DrawText("Registration Submitted!", SW/2-145,150,22,COL_BTN_GREEN);
    DrawText("Your account is pending admin approval.",SW/2-170,200,16,COL_TEXT_DARK);
    DrawText("Once approved you can login normally.",SW/2-160,225,16,COL_TEXT_DARK);
    if(DrawButton("Go to Login",{(float)(SW/2-70),310,140,40},COL_BTN_PURPLE,WHITE,16))
        currentScreen=SCR_LOGIN;
}

// ============================================================
//  STUDENT DASHBOARD
// ============================================================
void DrawStudentDashboard(){
    DrawText("Student Dashboard", 20, 70, 32, COL_BTN_PURPLE);
    DrawText(("Welcome, "+loggedName).c_str(), 20, 100, 20, COL_TEXT_DARK);

    // stat cards
    auto statCard=[&](Rectangle r, Color c, const char* n, const char* v){
        DrawRectangleRounded(r,0.15f,8,c);
        DrawText(n,(int)r.x+12,(int)r.y+10,13,WHITE);
        DrawText(v,(int)r.x+12,(int)r.y+34,26,WHITE);
    };

    // count this student's data
    int myEnroll=0; float myGrade=0; int myTasks=0;
    for(int i=0;i<gEnrollCount;i++)
        if(string(gEnrollments[i].courseID).find(loggedEmail)!=string::npos||true) myEnroll++;
    for(int i=0;i<gGradeCount;i++) myGrade+=gGrades[i].grade;
    float avgG = gGradeCount>0 ? myGrade/gGradeCount : 0;
    for(int i=0;i<gTaskCount;i++) if(!gTasks[i].isDone) myTasks++;

    statCard({20,130,180,80},COL_BTN_PURPLE,"Enrolled Courses",to_string(gCourseCount).c_str());
    statCard({210,130,180,80},COL_BTN_GREEN,"Avg Grade",(to_string((int)avgG)+"%").c_str());
    statCard({400,130,180,80},COL_BTN_BLUE,"Pending Tasks",to_string(myTasks).c_str());
    statCard({590,130,180,80},COL_ACCENT,"Zoom Links",to_string(gZoomCount).c_str());

    // Navigation buttons
    DrawText("Quick Access", 20, 232, 22, COL_TEXT_DARK);
    struct NavBtn{ const char* lbl; Screen s; Color c; };
    NavBtn nav[]={
        {"My Courses",SCR_COURSES,COL_BTN_PURPLE},
        {"Gradebook",SCR_GRADEBOOK,COL_BTN_GREEN},
        {"Tasks",SCR_TASKS,COL_BTN_BLUE},
        {"Zoom Links",SCR_ZOOM,COL_ACCENT},
        {"Take Quiz",SCR_QUIZ,COL_CARD_ADMIN1}
    };
    for(int i=0;i<5;i++)
        if(DrawButton(nav[i].lbl,{(float)(20+i*210),260,190,44},nav[i].c,WHITE,15))
            currentScreen=nav[i].s;

    // Recent courses table
    DrawText("Available Courses", 20, 325, 22, COL_TEXT_DARK);
    DrawRectangle(20,348,SW-40,28,COL_BTN_PURPLE);
    DrawText("ID",30,354,16,WHITE); DrawText("Title",120,354,16,WHITE);
    DrawText("Type",500,354,16,WHITE); DrawText("Status",650,354,16,WHITE);
    DrawText("Price",800,354,16,WHITE);

    HandleScroll(listScroll,(float)max(0,gCourseCount*30-200));
    BeginScissorMode(20,378,SW-40,250);
    for(int i=0;i<gCourseCount;i++){
        float y = 378 + i*30 - listScroll;
        if(y < 370 || y > 630) continue;
        Color row = (i%2==0)?WHITE:COL_ROW_ALT;
        DrawRectangle(20,(int)y,SW-40,28,row);
        DrawText(gCourses[i].courseID,30,(int)y+7,15,COL_TEXT_DARK);
        DrawText(gCourses[i].title,120,(int)y+7,15,COL_TEXT_DARK);
        DrawText(gCourses[i].type,500,(int)y+7,15,COL_TEXT_DARK);
        DrawText(gCourses[i].status,650,(int)y+7,15,
                 string(gCourses[i].status)=="Approved"?COL_BTN_GREEN:COL_BTN_RED);
        DrawText(gCourses[i].price>0 ? ("Rs."+to_string((int)gCourses[i].price)).c_str() : "FREE",
                 800,(int)y+7,13,COL_TEXT_DARK);
    }
    EndScissorMode();
}

// ============================================================
//  TEACHER DASHBOARD
// ============================================================
void DrawTeacherDashboard(){
    DrawText("Teacher Dashboard", 20, 70, 32, COL_CARD_TEACH);
    DrawText(("Welcome, "+loggedName).c_str(), 20, 100, 20, COL_TEXT_DARK);

    // stat cards
    auto statCard=[&](Rectangle r, Color c, const char* n, const char* v){
        DrawRectangleRounded(r,0.15f,8,c);
        DrawText(n,(int)r.x+12,(int)r.y+10,13,WHITE);
        DrawText(v,(int)r.x+12,(int)r.y+34,26,WHITE);
    };
    statCard({20,130,200,80},COL_CARD_TEACH,"Total Courses",to_string(gCourseCount).c_str());
    statCard({230,130,200,80},COL_BTN_BLUE,"Submissions",to_string(gSubCount).c_str());
    statCard({440,130,200,80},COL_BTN_PURPLE,"Zoom Links",to_string(gZoomCount).c_str());

    DrawText("Teacher Tools", 20, 230, 22, COL_TEXT_DARK);
    struct NavBtn{ const char* lbl; Screen s; Color c; };
    NavBtn nav[]={
        {"My Courses",SCR_TEACHER_COURSES,COL_CARD_TEACH},
        {"Upload Material",SCR_UPLOAD_MATERIAL,COL_BTN_BLUE},
        {"Upload Quiz",SCR_UPLOAD_QUIZ,COL_BTN_PURPLE},
        {"View Submissions",SCR_VIEW_SUBMISSIONS,COL_ACCENT},
        {"Gradebook",SCR_TEACHER_GRADEBOOK,COL_BTN_GREEN}
    };
    for(int i=0;i<5;i++)
        if(DrawButton(nav[i].lbl,{(float)(20+i*210),258,190,44},nav[i].c,WHITE,14))
            currentScreen=nav[i].s;

    // Courses table
    DrawText("My Courses", 20, 322, 17, COL_TEXT_DARK);
    DrawRectangle(20,346,SW-40,28,COL_CARD_TEACH);
    DrawText("ID",30,352,14,WHITE); DrawText("Title",130,352,14,WHITE);
    DrawText("Status",550,352,14,WHITE); DrawText("Price",700,352,14,WHITE);

    HandleScroll(listScroll,(float)max(0,gCourseCount*30-250));
    BeginScissorMode(20,376,SW-40,270);
    for(int i=0;i<gCourseCount;i++){
        float y=376+i*30-listScroll;
        if(y<370||y>650) continue;
        Color row=(i%2==0)?WHITE:COL_ROW_ALT;
        DrawRectangle(20,(int)y,SW-40,28,row);
        DrawText(gCourses[i].courseID,30,(int)y+7,13,COL_TEXT_DARK);
        DrawText(gCourses[i].title,130,(int)y+7,13,COL_TEXT_DARK);
        Color sc=string(gCourses[i].status)=="Approved"?COL_BTN_GREEN:
                 string(gCourses[i].status)=="Rejected"?COL_BTN_RED:COL_CARD_ADMIN1;
        DrawText(gCourses[i].status,550,(int)y+7,13,sc);
        DrawText(gCourses[i].price>0?("Rs."+to_string((int)gCourses[i].price)).c_str():"FREE",
                 700,(int)y+7,13,COL_TEXT_DARK);
    }
    EndScissorMode();
}

// ============================================================
//  ADMIN DASHBOARD
// ============================================================
void DrawAdminDashboard(){
    DrawText("Admin Dashboard", 20, 70, 32, COL_CARD_ADMIN1);
    DrawText(("Welcome, "+loggedName+" — Admin Control Panel").c_str(),20,100,20,COL_TEXT_DARK);

    // stats
    auto statCard=[&](Rectangle r, Color c, const char* n, const char* v){
        DrawRectangleRounded(r,0.15f,8,c);
        DrawText(n,(int)r.x+12,(int)r.y+10,13,WHITE);
        DrawText(v,(int)r.x+12,(int)r.y+34,26,WHITE);
    };
    int pendingCourses=0;
    for(int i=0;i<gCourseCount;i++) if(string(gCourses[i].status)=="Pending") pendingCourses++;
    statCard({20,130,200,80},COL_BTN_PURPLE,"Total Users",to_string(gUserCount).c_str());
    statCard({230,130,200,80},COL_CARD_ADMIN1,"Total Courses",to_string(gCourseCount).c_str());
    statCard({440,130,200,80},COL_BTN_RED,"Pending Approval",to_string(pendingCourses).c_str());
    statCard({650,130,200,80},COL_BTN_GREEN,"Enrollments",to_string(gEnrollCount).c_str());

    DrawText("Admin Tools", 20, 230, 22, COL_TEXT_DARK);
    struct NavBtn{ const char* lbl; Screen s; Color c; };
    NavBtn nav[]={
        {"Approve Courses",SCR_ADMIN_APPROVE_COURSES,COL_BTN_PURPLE},
        {"Approve Regs",SCR_ADMIN_APPROVE_REGS,COL_CARD_ADMIN1},
        {"Manage Users",SCR_ADMIN_MANAGE_USERS,COL_BTN_BLUE},
        {"Subscriptions",SCR_ADMIN_SUBSCRIPTIONS,COL_CARD_TEACH},
        {"Enrollments",SCR_ADMIN_ENROLLMENTS,COL_ACCENT}
    };
    for(int i=0;i<5;i++)
        if(DrawButton(nav[i].lbl,{(float)(20+i*210),258,190,44},nav[i].c,WHITE,13))
            currentScreen=nav[i].s;

    // Users table
    DrawText("All Users", 20, 322, 17, COL_TEXT_DARK);
    DrawRectangle(20,346,SW-40,28,COL_BTN_PURPLE);
    DrawText("ID",30,352,14,WHITE); DrawText("Name",130,352,14,WHITE);
    DrawText("Email",360,352,14,WHITE); DrawText("Role",620,352,14,WHITE);
    DrawText("Active",780,352,14,WHITE);

    HandleScroll(listScroll,(float)max(0,gUserCount*30-270));
    BeginScissorMode(20,376,SW-40,270);
    for(int i=0;i<gUserCount;i++){
        float y=376+i*30-listScroll;
        if(y<370||y>650) continue;
        Color row=(i%2==0)?WHITE:COL_ROW_ALT;
        DrawRectangle(20,(int)y,SW-40,28,row);
        DrawText(gUsers[i].userID,30,(int)y+7,13,COL_TEXT_DARK);
        DrawText(gUsers[i].name,130,(int)y+7,13,COL_TEXT_DARK);
        DrawText(gUsers[i].email,360,(int)y+7,13,COL_TEXT_DARK);
        DrawText(gUsers[i].role,620,(int)y+7,13,COL_BTN_BLUE);
        DrawText(gUsers[i].isActive?"Yes":"No",780,(int)y+7,13,
                 gUsers[i].isActive?COL_BTN_GREEN:COL_BTN_RED);
    }
    EndScissorMode();
}

// ============================================================
//  GRADEBOOK (Student view)
// ============================================================
void DrawGradebook(){
    DrawText("My Gradebook", 20, 70, 24, COL_BTN_GREEN);

    DrawRectangle(20,110,SW-40,28,COL_BTN_GREEN);
    DrawText("Course ID",30,116,14,WHITE);
    DrawText("Student ID",200,116,14,WHITE);
    DrawText("Grade",400,116,14,WHITE);
    DrawText("Result",550,116,14,WHITE);

    HandleScroll(listScroll,(float)max(0,gGradeCount*34-500));
    BeginScissorMode(20,140,SW-40,500);
    float total=0;
    for(int i=0;i<gGradeCount;i++){
        float y=140+i*34-listScroll;
        if(y<133||y>645) continue;
        Color row=(i%2==0)?WHITE:COL_ROW_ALT;
        DrawRectangle(20,(int)y,SW-40,30,row);
        DrawText(gGrades[i].courseID,30,(int)y+8,14,COL_TEXT_DARK);
        DrawText(to_string(gGrades[i].studentID).c_str(),200,(int)y+8,14,COL_TEXT_DARK);
        DrawText((to_string((int)gGrades[i].grade)+"%").c_str(),400,(int)y+8,14,COL_TEXT_DARK);
        const char* res = gGrades[i].grade>=50?"PASS":"FAIL";
        DrawText(res,550,(int)y+8,14,gGrades[i].grade>=50?COL_BTN_GREEN:COL_BTN_RED);
        total+=gGrades[i].grade;
    }
    EndScissorMode();

    if(gGradeCount>0){
        float avg=total/gGradeCount;
        DrawText(("Class Average: "+to_string((int)avg)+"%").c_str(),20,648,16,COL_TEXT_DARK);
        const char* badge = avg>=90?"PLATINUM":avg>=75?"GOLD":avg>=60?"SILVER":avg>=40?"BRONZE":"NONE";
        DrawText(("Badge: "+string(badge)).c_str(),250,648,16,COL_BTN_PURPLE);
    }
}

// ============================================================
//  MY COURSES
// ============================================================
void DrawMyCourses(){
    DrawText("Available Courses", 20, 70, 24, COL_BTN_PURPLE);

    // Enroll form
    DrawPanel({20,100,SW-40,80},COL_ROW_ALT,10.f);
    DrawInputBox("Course ID to Enroll",{30,130,300,34},inp0,128,0);
    if(DrawButton("ENROLL",{340,130,110,34},COL_BTN_GREEN,WHITE,14)){
        if(strlen(inp0)<2){ setFeedback("Enter a course ID",false); }
        else {
            EnrollmentRecord e; memset(&e,0,sizeof(e));
            e.enrollmentID=gEnrollCount+1;
            e.studentID=1;
            strncpy(e.courseID,inp0,19);
            strncpy(e.enrolledDate,"2025-01-01",19);
            e.paymentStatus=0; e.isConfirmed=1;
            gEnrollments[gEnrollCount++]=e;
            fh->saveEnrollments(gEnrollments,gEnrollCount);
            fh->logActivity(loggedName.c_str(),"Enroll",inp0);
            clearInputs();
            setFeedback("Enrolled successfully!",true);
        }
    }

    // Table
    DrawRectangle(20,192,SW-40,28,COL_BTN_PURPLE);
    DrawText("Course ID",30,198,14,WHITE); DrawText("Title",160,198,14,WHITE);
    DrawText("Type",500,198,14,WHITE); DrawText("Price",650,198,14,WHITE);
    DrawText("Status",800,198,14,WHITE);

    HandleScroll(listScroll,(float)max(0,gCourseCount*32-450));
    BeginScissorMode(20,222,SW-40,430);
    for(int i=0;i<gCourseCount;i++){
        float y=222+i*32-listScroll;
        if(y<215||y>655) continue;
        Color row=(i%2==0)?WHITE:COL_ROW_ALT;
        DrawRectangle(20,(int)y,SW-40,28,row);
        DrawText(gCourses[i].courseID,30,(int)y+7,13,COL_TEXT_DARK);
        DrawText(gCourses[i].title,160,(int)y+7,13,COL_TEXT_DARK);
        DrawText(gCourses[i].type,500,(int)y+7,13,COL_TEXT_DARK);
        DrawText(gCourses[i].price>0?("Rs."+to_string((int)gCourses[i].price)).c_str():"FREE",650,(int)y+7,13,COL_TEXT_DARK);
        DrawText(gCourses[i].status,800,(int)y+7,13,
                 string(gCourses[i].status)=="Approved"?COL_BTN_GREEN:COL_BTN_RED);
    }
    EndScissorMode();
}

// ============================================================
//  TASKS
// ============================================================
void DrawTasks(){
    DrawText("My Tasks", 20, 70, 24, COL_BTN_BLUE);

    // Add task form
    DrawPanel({20,100,SW-40,90},COL_ROW_ALT,10.f);
    DrawInputBox("Task Title",  {30,130,260,34},inp0,128,0);
    DrawInputBox("Due Date",    {300,130,160,34},inp1,128,1);
    DrawInputBox("Priority(High/Medium/Low)",{470,130,220,34},inp2,128,2);
    if(DrawButton("ADD TASK",{700,130,130,34},COL_BTN_GREEN,WHITE,14)){
        if(strlen(inp0)<2||strlen(inp1)<2){ setFeedback("Fill title and date",false); }
        else {
            TaskRecord t; memset(&t,0,sizeof(t));
            t.taskID=gTaskCount+1; t.studentID=1;
            strncpy(t.title,inp0,99);
            strncpy(t.dueDate,inp1,19);
            strncpy(t.priority,strlen(inp2)>0?inp2:"Medium",9);
            t.isDone=0;
            gTasks[gTaskCount++]=t;
            fh->saveTasks(gTasks,gTaskCount);
            clearInputs();
            setFeedback("Task added!",true);
        }
    }

    // Table header
    DrawRectangle(20,202,SW-40,28,COL_BTN_BLUE);
    DrawText("ID",30,208,14,WHITE); DrawText("Title",80,208,14,WHITE);
    DrawText("Due",440,208,14,WHITE); DrawText("Priority",580,208,14,WHITE);
    DrawText("Done",720,208,14,WHITE); DrawText("Action",820,208,14,WHITE);

    HandleScroll(listScroll,(float)max(0,gTaskCount*36-430));
    BeginScissorMode(20,232,SW-40,420);
    for(int i=0;i<gTaskCount;i++){
        float y=232+i*36-listScroll;
        if(y<225||y>655) continue;
        Color row=(i%2==0)?WHITE:COL_ROW_ALT;
        DrawRectangle(20,(int)y,SW-40,32,row);
        DrawText(to_string(gTasks[i].taskID).c_str(),30,(int)y+8,13,COL_TEXT_DARK);
        DrawText(gTasks[i].title,80,(int)y+8,13,COL_TEXT_DARK);
        DrawText(gTasks[i].dueDate,440,(int)y+8,13,COL_TEXT_DARK);
        Color pc = string(gTasks[i].priority)=="High"?COL_BTN_RED:
                   string(gTasks[i].priority)=="Medium"?COL_CARD_ADMIN1:COL_BTN_GREEN;
        DrawText(gTasks[i].priority,580,(int)y+8,13,pc);
        DrawText(gTasks[i].isDone?"YES":"NO",720,(int)y+8,13,
                 gTasks[i].isDone?COL_BTN_GREEN:COL_BTN_RED);
        if(!gTasks[i].isDone){
            if(DrawButton("Done",{820,(float)y+4,80,26},COL_BTN_GREEN,WHITE,12)){
                gTasks[i].isDone=1;
                fh->saveTasks(gTasks,gTaskCount);
                setFeedback("Task marked done!",true);
            }
        }
    }
    EndScissorMode();
}

// ============================================================
//  ZOOM LINKS
// ============================================================
void DrawZoomLinks(){
    DrawText("Zoom Links", 20, 70, 24, COL_BTN_PURPLE);

    // Add link form
    DrawPanel({20,100,SW-40,100},COL_ROW_ALT,10.f);
    DrawInputBox("Teacher Name",{30,130,200,34},inp0,128,0);
    DrawInputBox("Section",    {240,130,120,34},inp1,128,1);
    DrawInputBox("Course",     {370,130,200,34},inp2,128,2);
    DrawInputBox("URL",        {30,170,400,34},inp3,128,3);     // second row
    DrawInputBox("Time",       {440,170,200,34},inp4,128,4);
    if(DrawButton("ADD",{650,152,100,52},COL_BTN_BLUE,WHITE,14)){
        if(strlen(inp0)<2||strlen(inp3)<4){ setFeedback("Fill name and URL",false); }
        else {
            ZoomLinkRecord z; memset(&z,0,sizeof(z));
            z.linkID=gZoomCount+1;
            strncpy(z.teacherName,inp0,59);
            strncpy(z.section,inp1,19);
            strncpy(z.courseTitle,inp2,99);
            strncpy(z.url,inp3,199);
            strncpy(z.scheduledTime,inp4,29);
            gZoom[gZoomCount++]=z;
            fh->saveZoomLinks(gZoom,gZoomCount);
            clearInputs();
            setFeedback("Zoom link added!",true);
        }
    }

    // Table
    DrawRectangle(20,212,SW-40,28,COL_BTN_PURPLE);
    DrawText("ID",30,218,14,WHITE); DrawText("Teacher",80,218,14,WHITE);
    DrawText("Section",270,218,14,WHITE); DrawText("Course",380,218,14,WHITE);
    DrawText("Time",650,218,14,WHITE); DrawText("URL",820,218,14,WHITE);

    HandleScroll(listScroll,(float)max(0,gZoomCount*32-400));
    BeginScissorMode(20,242,SW-40,400);
    for(int i=0;i<gZoomCount;i++){
        float y=242+i*32-listScroll;
        if(y<235||y>645) continue;
        Color row=(i%2==0)?WHITE:COL_ROW_ALT;
        DrawRectangle(20,(int)y,SW-40,28,row);
        DrawText(to_string(gZoom[i].linkID).c_str(),30,(int)y+7,13,COL_TEXT_DARK);
        DrawText(gZoom[i].teacherName,80,(int)y+7,13,COL_TEXT_DARK);
        DrawText(gZoom[i].section,270,(int)y+7,13,COL_TEXT_DARK);
        DrawText(gZoom[i].courseTitle,380,(int)y+7,13,COL_TEXT_DARK);
        DrawText(gZoom[i].scheduledTime,650,(int)y+7,13,COL_TEXT_DARK);
        // truncate URL
        string u=string(gZoom[i].url); if(u.size()>20) u=u.substr(0,20)+"...";
        DrawText(u.c_str(),820,(int)y+7,13,COL_BTN_BLUE);
    }
    EndScissorMode();
}

// ============================================================
//  QUIZ (simple demo attempt)
// ============================================================
static int quizAnswers[10]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
static bool quizSubmitted=false;
static float quizScore=0;

void DrawQuiz(){
    DrawText("Take a Quiz", 20, 70, 24, COL_BTN_BLUE);

    if(quizSubmitted){
        DrawText(("Your Score: "+to_string((int)quizScore)+" / 30").c_str(),SW/2-100,200,28,COL_BTN_GREEN);
        const char* badge = quizScore>=27?"PLATINUM":quizScore>=22?"GOLD":quizScore>=18?"SILVER":"BRONZE";
        DrawText(("Badge Earned: "+string(badge)).c_str(),SW/2-90,250,22,COL_BTN_PURPLE);
        if(DrawButton("Try Again",{(float)(SW/2-70),320,140,40},COL_BTN_BLUE,WHITE,16)){
            quizSubmitted=false; quizScore=0;
            for(int i=0;i<10;i++) quizAnswers[i]=-1;
        }
        return;
    }

    // 3 sample questions
    struct Q{ const char* q; const char* opts[4]; int ans; int marks; };
    Q qs[3]={
        {"What does OOP stand for?",
         {"Object Oriented Programming","Only One Program","Open Operation Platform","None"},0,10},
        {"Which keyword creates a class in C++?",
         {"struct","object","class","define"},2,10},
        {"What is inheritance in OOP?",
         {"Copying code","Class getting properties of another","Using pointers","File reading"},1,10}
    };

    for(int i=0;i<3;i++){
        int qy = 110 + i*160;
        DrawText(qs[i].q, 30, qy, 16, COL_TEXT_DARK);
        for(int j=0;j<4;j++){
            bool sel=(quizAnswers[i]==j);
           Color bc = sel ? COL_BTN_PURPLE : Color{200, 190, 230, 255};
            if(DrawButton(qs[i].opts[j],{30,(float)(qy+30+j*28),500,24},bc,sel?WHITE:COL_TEXT_DARK,13))
                quizAnswers[i]=j;
        }
    }

    if(DrawButton("SUBMIT QUIZ",{(float)(SW/2-80),590,160,42},COL_ACCENT,WHITE,16)){
        quizScore=0;
        for(int i=0;i<3;i++)
            if(quizAnswers[i]==qs[i].ans) quizScore+=qs[i].marks;
        quizSubmitted=true;
        // log it
        fh->logActivity(loggedName.c_str(),"QuizAttempt",("Score:"+to_string((int)quizScore)).c_str());
    }
}

// ============================================================
//  TEACHER: MY COURSES + ADD COURSE
// ============================================================
void DrawTeacherCourses(){
    DrawText("Teacher: My Courses", 20, 70, 24, COL_CARD_TEACH);

    // Add form
    DrawPanel({20,100,SW-40,100},COL_ROW_ALT,10.f);
    DrawInputBox("Course ID",{30,130,120,34},inp0,128,0);
    DrawInputBox("Title",{160,130,280,34},inp1,128,1);
    DrawInputBox("Price(0=Free)",{450,130,150,34},inp2,128,2);
    DrawInputBox("Type(Free/Paid)",{610,130,160,34},inp3,128,3);
    DrawInputBox("Description",{30,170,580,34},inp4,128,4);
    if(DrawButton("ADD COURSE",{780,148,130,54},COL_CARD_TEACH,WHITE,13)){
        if(strlen(inp0)<1||strlen(inp1)<2){ setFeedback("Fill ID and Title",false); }
        else {
            CourseRecord c; memset(&c,0,sizeof(c));
            strncpy(c.courseID,inp0,19);
            strncpy(c.title,inp1,99);
            strncpy(c.description,inp4,199);
            c.price=strlen(inp2)>0?atof(inp2):0;
            c.teacherID=1;
            strncpy(c.status,"Pending",19);
            strncpy(c.type,strlen(inp3)>0?inp3:"Free",9);
            c.isApproved=0;
            gCourses[gCourseCount++]=c;
            fh->saveCourses(gCourses,gCourseCount);
            fh->logActivity(loggedName.c_str(),"AddCourse",inp1);
            clearInputs();
            setFeedback("Course submitted for approval!",true);
        }
    }

    // Table
    DrawRectangle(20,212,SW-40,28,COL_CARD_TEACH);
    DrawText("ID",30,218,14,WHITE); DrawText("Title",130,218,14,WHITE);
    DrawText("Type",500,218,14,WHITE); DrawText("Status",650,218,14,WHITE);
    DrawText("Price",820,218,14,WHITE);

    HandleScroll(listScroll,(float)max(0,gCourseCount*30-400));
    BeginScissorMode(20,242,SW-40,400);
    for(int i=0;i<gCourseCount;i++){
        float y=242+i*30-listScroll;
        if(y<235||y>645) continue;
        Color row=(i%2==0)?WHITE:COL_ROW_ALT;
        DrawRectangle(20,(int)y,SW-40,26,row);
        DrawText(gCourses[i].courseID,30,(int)y+6,13,COL_TEXT_DARK);
        DrawText(gCourses[i].title,130,(int)y+6,13,COL_TEXT_DARK);
        DrawText(gCourses[i].type,500,(int)y+6,13,COL_TEXT_DARK);
        DrawText(gCourses[i].status,650,(int)y+6,13,
                 string(gCourses[i].status)=="Approved"?COL_BTN_GREEN:
                 string(gCourses[i].status)=="Rejected"?COL_BTN_RED:COL_CARD_ADMIN1);
        DrawText(gCourses[i].price>0?("Rs."+to_string((int)gCourses[i].price)).c_str():"FREE",820,(int)y+6,13,COL_TEXT_DARK);
    }
    EndScissorMode();
}

// ============================================================
//  UPLOAD MATERIAL
// ============================================================
void DrawUploadMaterial(){
    DrawText("Upload Course Material", 20, 70, 24, COL_BTN_BLUE);
    DrawPanel({20,100,SW-40,120},COL_ROW_ALT,10.f);
    DrawInputBox("Material Title",{30,130,280,34},inp0,128,0);
    DrawInputBox("File Type (PDF/Video)",{320,130,220,34},inp1,128,1);
    DrawInputBox("File Path",{30,178,400,34},inp2,128,2);
    DrawInputBox("Course ID",{440,178,150,34},inp3,128,3);

    if(DrawButton("UPLOAD",{600,148,120,62},COL_BTN_BLUE,WHITE,15)){
        if(strlen(inp0)<2||strlen(inp2)<2){ setFeedback("Fill title and path",false); }
        else {
            AssignmentRecord a; memset(&a,0,sizeof(a));
            a.assignmentID=gAssignCount+1;
            strncpy(a.title,inp0,99);
            strncpy(a.description,inp2,199);
            strncpy(a.deadline,"N/A",19);
            a.maxMarks=0;
            a.courseID=strlen(inp3)>0?atoi(inp3):1;
            gAssigns[gAssignCount++]=a;
            fh->saveAssignments(gAssigns,gAssignCount);
            fh->logActivity(loggedName.c_str(),"UploadMaterial",inp0);
            clearInputs();
            setFeedback("Material uploaded!",true);
        }
    }

    // list assignments as materials
    DrawText("Uploaded Materials", 20, 235, 17, COL_TEXT_DARK);
    DrawRectangle(20,258,SW-40,28,COL_BTN_BLUE);
    DrawText("ID",30,264,14,WHITE); DrawText("Title",100,264,14,WHITE);
    DrawText("Course ID",700,264,14,WHITE);

    HandleScroll(listScroll,(float)max(0,gAssignCount*30-360));
    BeginScissorMode(20,288,SW-40,360);
    for(int i=0;i<gAssignCount;i++){
        float y=288+i*30-listScroll;
        if(y<282||y>650) continue;
        Color row=(i%2==0)?WHITE:COL_ROW_ALT;
        DrawRectangle(20,(int)y,SW-40,26,row);
        DrawText(to_string(gAssigns[i].assignmentID).c_str(),30,(int)y+6,13,COL_TEXT_DARK);
        DrawText(gAssigns[i].title,100,(int)y+6,13,COL_TEXT_DARK);
        DrawText(to_string(gAssigns[i].courseID).c_str(),700,(int)y+6,13,COL_TEXT_DARK);
    }
    EndScissorMode();
}

// ============================================================
//  UPLOAD QUIZ / ASSIGNMENT
// ============================================================
void DrawUploadQuiz(){
    DrawText("Upload Quiz / Assignment", 20, 70, 24, COL_BTN_PURPLE);
    DrawPanel({20,100,SW-40,130},COL_ROW_ALT,10.f);
    DrawInputBox("Assignment Title",  {30,130,300,34},inp0,128,0);
    DrawInputBox("Description",       {340,130,380,34},inp1,128,1);
    DrawInputBox("Deadline (YYYY-MM-DD)",{30,178,220,34},inp2,128,2);
    DrawInputBox("Max Marks",         {260,178,140,34},inp3,128,3);
    DrawInputBox("Course ID",         {410,178,120,34},inp4,128,4);

    if(DrawButton("CREATE",{540,148,120,62},COL_BTN_PURPLE,WHITE,15)){
        if(strlen(inp0)<2){ setFeedback("Enter a title",false); }
        else {
            AssignmentRecord a; memset(&a,0,sizeof(a));
            a.assignmentID=gAssignCount+1;
            strncpy(a.title,inp0,99);
            strncpy(a.description,inp1,199);
            strncpy(a.deadline,strlen(inp2)>0?inp2:"TBD",19);
            a.maxMarks=strlen(inp3)>0?atoi(inp3):100;
            a.courseID=strlen(inp4)>0?atoi(inp4):1;
            gAssigns[gAssignCount++]=a;
            fh->saveAssignments(gAssigns,gAssignCount);
            fh->logActivity(loggedName.c_str(),"CreateAssignment",inp0);
            clearInputs();
            setFeedback("Assignment created!",true);
        }
    }

    // table
    DrawText("All Assignments", 20, 245, 17, COL_TEXT_DARK);
    DrawRectangle(20,268,SW-40,28,COL_BTN_PURPLE);
    DrawText("ID",30,274,14,WHITE); DrawText("Title",100,274,14,WHITE);
    DrawText("Deadline",500,274,14,WHITE); DrawText("MaxMarks",680,274,14,WHITE);
    DrawText("CourseID",830,274,14,WHITE);

    HandleScroll(listScroll,(float)max(0,gAssignCount*30-350));
    BeginScissorMode(20,298,SW-40,350);
    for(int i=0;i<gAssignCount;i++){
        float y=298+i*30-listScroll;
        if(y<292||y>650) continue;
        Color row=(i%2==0)?WHITE:COL_ROW_ALT;
        DrawRectangle(20,(int)y,SW-40,26,row);
        DrawText(to_string(gAssigns[i].assignmentID).c_str(),30,(int)y+6,13,COL_TEXT_DARK);
        DrawText(gAssigns[i].title,100,(int)y+6,13,COL_TEXT_DARK);
        DrawText(gAssigns[i].deadline,500,(int)y+6,13,COL_TEXT_DARK);
        DrawText(to_string(gAssigns[i].maxMarks).c_str(),680,(int)y+6,13,COL_TEXT_DARK);
        DrawText(to_string(gAssigns[i].courseID).c_str(),830,(int)y+6,13,COL_TEXT_DARK);
    }
    EndScissorMode();
}

// ============================================================
//  VIEW SUBMISSIONS
// ============================================================
void DrawViewSubmissions(){
    DrawText("Student Submissions", 20, 70, 24, COL_ACCENT);

    // Grade a submission
    DrawPanel({20,100,SW-40,80},COL_ROW_ALT,10.f);
    DrawInputBox("Submission ID",{30,130,160,34},inp0,128,0);
    DrawInputBox("Grade (0-100)", {200,130,160,34},inp1,128,1);
    if(DrawButton("GRADE",{370,130,100,34},COL_ACCENT,WHITE,14)){
        int sid=atoi(inp0); float gr=atof(inp1);
        bool found=false;
        for(int i=0;i<gSubCount;i++){
            if(gSubs[i].submissionID==sid){
                gSubs[i].grade=gr; gSubs[i].isGraded=1;
                fh->saveSubmissions(gSubs,gSubCount);
                // also update gradebook
                GradeRecord g; memset(&g,0,sizeof(g));
                strncpy(g.courseID,"AUTO",19);
                g.studentID=gSubs[i].studentID; g.grade=gr;
                gGrades[gGradeCount++]=g;
                fh->saveGrades(gGrades,gGradeCount);
                fh->logActivity(loggedName.c_str(),"GradeSubmission",inp0);
                clearInputs(); setFeedback("Graded successfully!",true); found=true; break;
            }
        }
        if(!found) setFeedback("Submission ID not found",false);
    }

    DrawRectangle(20,192,SW-40,28,COL_ACCENT);
    DrawText("Sub ID",30,198,14,WHITE); DrawText("Student",110,198,14,WHITE);
    DrawText("Assignment",210,198,14,WHITE); DrawText("Date",450,198,14,WHITE);
    DrawText("Graded",620,198,14,WHITE); DrawText("Grade",740,198,14,WHITE);

    HandleScroll(listScroll,(float)max(0,gSubCount*30-420));
    BeginScissorMode(20,222,SW-40,420);
    for(int i=0;i<gSubCount;i++){
        float y=222+i*30-listScroll;
        if(y<215||y>645) continue;
        Color row=(i%2==0)?WHITE:COL_ROW_ALT;
        DrawRectangle(20,(int)y,SW-40,26,row);
        DrawText(to_string(gSubs[i].submissionID).c_str(),30,(int)y+6,13,COL_TEXT_DARK);
        DrawText(to_string(gSubs[i].studentID).c_str(),110,(int)y+6,13,COL_TEXT_DARK);
        DrawText(to_string(gSubs[i].assignmentID).c_str(),210,(int)y+6,13,COL_TEXT_DARK);
        DrawText(gSubs[i].submittedDate,450,(int)y+6,13,COL_TEXT_DARK);
        DrawText(gSubs[i].isGraded?"Yes":"No",620,(int)y+6,13,
                 gSubs[i].isGraded?COL_BTN_GREEN:COL_BTN_RED);
        if(gSubs[i].isGraded)
            DrawText(to_string((int)gSubs[i].grade).c_str(),740,(int)y+6,13,COL_TEXT_DARK);
    }
    EndScissorMode();
}

// ============================================================
//  TEACHER GRADEBOOK
// ============================================================
void DrawTeacherGradebook(){
    DrawText("Teacher: Gradebook", 20, 70, 24, COL_BTN_GREEN);

    // Add grade manually
    DrawPanel({20,100,SW-40,80},COL_ROW_ALT,10.f);
    DrawInputBox("Course ID",  {30,130,160,34},inp0,128,0);
    DrawInputBox("Student ID", {200,130,150,34},inp1,128,1);
    DrawInputBox("Grade",      {360,130,120,34},inp2,128,2);
    if(DrawButton("ADD GRADE",{490,130,120,34},COL_BTN_GREEN,WHITE,13)){
        if(strlen(inp0)<1||strlen(inp1)<1){ setFeedback("Fill all fields",false); }
        else {
            GradeRecord g; memset(&g,0,sizeof(g));
            strncpy(g.courseID,inp0,19);
            g.studentID=atoi(inp1); g.grade=atof(inp2);
            gGrades[gGradeCount++]=g;
            fh->saveGrades(gGrades,gGradeCount);
            fh->logActivity(loggedName.c_str(),"AddGrade",inp0);
            clearInputs(); setFeedback("Grade recorded!",true);
        }
    }

    DrawRectangle(20,192,SW-40,28,COL_BTN_GREEN);
    DrawText("Course",30,198,14,WHITE); DrawText("Student ID",220,198,14,WHITE);
    DrawText("Grade",420,198,14,WHITE); DrawText("Result",580,198,14,WHITE);

    float total=0; int cnt=0;
    HandleScroll(listScroll,(float)max(0,gGradeCount*30-400));
    BeginScissorMode(20,222,SW-40,400);
    for(int i=0;i<gGradeCount;i++){
        float y=222+i*30-listScroll;
        if(y<215||y>625) continue;
        Color row=(i%2==0)?WHITE:COL_ROW_ALT;
        DrawRectangle(20,(int)y,SW-40,26,row);
        DrawText(gGrades[i].courseID,30,(int)y+6,13,COL_TEXT_DARK);
        DrawText(to_string(gGrades[i].studentID).c_str(),220,(int)y+6,13,COL_TEXT_DARK);
        DrawText((to_string((int)gGrades[i].grade)+"%").c_str(),420,(int)y+6,13,COL_TEXT_DARK);
        DrawText(gGrades[i].grade>=50?"PASS":"FAIL",580,(int)y+6,13,
                 gGrades[i].grade>=50?COL_BTN_GREEN:COL_BTN_RED);
        total+=gGrades[i].grade; cnt++;
    }
    EndScissorMode();
    if(cnt>0) DrawText(("Class Average: "+to_string((int)(total/cnt))+"%").c_str(),20,630,16,COL_TEXT_DARK);
}

// ============================================================
//  ADMIN: APPROVE COURSES
// ============================================================
void DrawAdminApproveCourses(){
    DrawText("Admin: Approve Courses", 20, 70, 24, COL_BTN_PURPLE);

    DrawRectangle(20,100,SW-40,28,COL_BTN_PURPLE);
    DrawText("ID",30,106,14,WHITE); DrawText("Title",120,106,14,WHITE);
    DrawText("Type",500,106,14,WHITE); DrawText("Status",640,106,14,WHITE);
    DrawText("Approve",790,106,14,WHITE); DrawText("Reject",890,106,14,WHITE);

    HandleScroll(listScroll,(float)max(0,gCourseCount*36-540));
    BeginScissorMode(20,130,SW-40,520);
    for(int i=0;i<gCourseCount;i++){
        float y=130+i*36-listScroll;
        if(y<124||y>655) continue;
        Color row=(i%2==0)?WHITE:COL_ROW_ALT;
        DrawRectangle(20,(int)y,SW-40,32,row);
        DrawText(gCourses[i].courseID,30,(int)y+8,13,COL_TEXT_DARK);
        DrawText(gCourses[i].title,120,(int)y+8,13,COL_TEXT_DARK);
        DrawText(gCourses[i].type,500,(int)y+8,13,COL_TEXT_DARK);
        Color sc=string(gCourses[i].status)=="Approved"?COL_BTN_GREEN:
                 string(gCourses[i].status)=="Rejected"?COL_BTN_RED:COL_CARD_ADMIN1;
        DrawText(gCourses[i].status,640,(int)y+8,13,sc);

        if(string(gCourses[i].status)=="Pending"){
            if(DrawButton("Approve",{790,(float)y+4,90,26},COL_BTN_GREEN,WHITE,12)){
                strncpy(gCourses[i].status,"Approved",19);
                gCourses[i].isApproved=1;
                fh->saveCourses(gCourses,gCourseCount);
                fh->logActivity("Admin","ApproveCourse",gCourses[i].courseID);
                setFeedback("Course approved!",true);
            }
            if(DrawButton("Reject",{888,(float)y+4,80,26},COL_BTN_RED,WHITE,12)){
                strncpy(gCourses[i].status,"Rejected",19);
                gCourses[i].isApproved=0;
                fh->saveCourses(gCourses,gCourseCount);
                fh->logActivity("Admin","RejectCourse",gCourses[i].courseID);
                setFeedback("Course rejected.",false);
            }
        }
    }
    EndScissorMode();
}

// ============================================================
//  ADMIN: APPROVE REGISTRATIONS
// ============================================================
void DrawAdminApproveRegs(){
    DrawText("Admin: Approve Registrations", 20, 70, 24, COL_CARD_ADMIN1);

    DrawRectangle(20,100,SW-40,28,COL_CARD_ADMIN1);
    DrawText("UserID",30,106,14,WHITE); DrawText("Name",160,106,14,WHITE);
    DrawText("Email",360,106,14,WHITE); DrawText("Role",600,106,14,WHITE);
    DrawText("Active",740,106,14,WHITE); DrawText("Action",860,106,14,WHITE);

    HandleScroll(listScroll,(float)max(0,gUserCount*36-530));
    BeginScissorMode(20,130,SW-40,520);
    for(int i=0;i<gUserCount;i++){
        float y=130+i*36-listScroll;
        if(y<124||y>655) continue;
        Color row=(i%2==0)?WHITE:COL_ROW_ALT;
        DrawRectangle(20,(int)y,SW-40,32,row);
        DrawText(gUsers[i].userID,30,(int)y+8,13,COL_TEXT_DARK);
        DrawText(gUsers[i].name,160,(int)y+8,13,COL_TEXT_DARK);
        DrawText(gUsers[i].email,360,(int)y+8,13,COL_TEXT_DARK);
        DrawText(gUsers[i].role,600,(int)y+8,13,COL_BTN_BLUE);
        DrawText(gUsers[i].isActive?"Active":"Inactive",740,(int)y+8,13,
                 gUsers[i].isActive?COL_BTN_GREEN:COL_BTN_RED);
        if(!gUsers[i].isActive){
            if(DrawButton("Activate",{860,(float)y+4,100,26},COL_BTN_GREEN,WHITE,12)){
                gUsers[i].isActive=1;
                fh->saveUsers(gUsers,gUserCount);
                fh->logActivity("Admin","ActivateUser",gUsers[i].userID);
                setFeedback("User activated!",true);
            }
        } else {
            if(DrawButton("Ban",{860,(float)y+4,76,26},COL_BTN_RED,WHITE,12)){
                gUsers[i].isActive=0;
                fh->saveUsers(gUsers,gUserCount);
                fh->logActivity("Admin","BanUser",gUsers[i].userID);
                setFeedback("User banned.",false);
            }
            if(DrawButton("Remove",{940,(float)y+4,76,26},{160,40,40,255},WHITE,11)){
                // shift all users left to remove this user
                for(int j=i; j<gUserCount-1; j++)
                    gUsers[j]=gUsers[j+1];
                gUserCount--;
                fh->saveUsers(gUsers,gUserCount);
                fh->logActivity("Admin","RemoveUser",gUsers[i].userID);
                setFeedback("User removed!",false);
                break;  // break out of loop since array shifted
            }
        }
    }
    EndScissorMode();
}

// ============================================================
//  ADMIN: MANAGE USERS
// ============================================================
void DrawAdminManageUsers(){
    DrawText("Admin: Manage Users", 20, 70, 24, COL_BTN_BLUE);

    // Reset password
    DrawPanel({20,100,SW-40,80},COL_ROW_ALT,10.f);
    DrawInputBox("User Email",    {30,130,260,34},inp0,128,0);
    DrawInputBox("New Password",  {300,130,220,34},inp1,128,1,true);
    if(DrawButton("RESET PW",{530,130,120,34},COL_BTN_BLUE,WHITE,13)){
        if(strlen(inp0)<3||strlen(inp1)<4){ setFeedback("Fill email and new password",false); }
        else {
            if(fh->changePassword(string(inp0),string(inp1))){
                fh->logActivity("Admin","ResetPassword",inp0);
                clearInputs(); setFeedback("Password reset!",true);
            } else setFeedback("Email not found",false);
        }
    }

    DrawRectangle(20,192,SW-40,28,COL_BTN_BLUE);
    DrawText("UserID",30,198,14,WHITE); DrawText("Name",160,198,14,WHITE);
    DrawText("Email",360,198,14,WHITE); DrawText("Role",620,198,14,WHITE);
    DrawText("Reg Date",760,198,14,WHITE);

    HandleScroll(listScroll,(float)max(0,gUserCount*30-420));
    BeginScissorMode(20,222,SW-40,420);
    for(int i=0;i<gUserCount;i++){
        float y=222+i*30-listScroll;
        if(y<215||y>645) continue;
        Color row=(i%2==0)?WHITE:COL_ROW_ALT;
        DrawRectangle(20,(int)y,SW-40,26,row);
        DrawText(gUsers[i].userID,30,(int)y+6,13,COL_TEXT_DARK);
        DrawText(gUsers[i].name,160,(int)y+6,13,COL_TEXT_DARK);
        DrawText(gUsers[i].email,360,(int)y+6,13,COL_TEXT_DARK);
        DrawText(gUsers[i].role,620,(int)y+6,13,COL_BTN_BLUE);
        DrawText(gUsers[i].registrationDate,760,(int)y+6,13,COL_TEXT_DARK);
    }
    EndScissorMode();
}

// ============================================================
//  ADMIN: SUBSCRIPTIONS
// ============================================================
void DrawAdminSubscriptions(){
   DrawText("Admin: Subscription Plans", 20, 70, 24, COL_CARD_TEACH);

    // Display 3 plans
    struct Plan{ const char* name; const char* price; const char* features; Color c; };
    Plan plans[]={
        {"Basic","Rs. 999/mo","Access to Free Courses | Community Forum | Email Support",{80,180,140,255}},
        {"Pro","Rs. 2999/mo","All Basic + Paid Courses | Quiz Access | Priority Support",{100,140,220,255}},
        {"Premium","Rs. 5999/mo","All Pro + 1-on-1 Sessions | Certificates | Early Access",{160,80,200,255}}
    };

    for(int i=0;i<3;i++){
        Rectangle r={(float)(30+i*360),(float)120,(float)340,(float)240};
        DrawRectangleRounded(r,0.1f,8,plans[i].c);
        DrawText(plans[i].name,(int)r.x+20,(int)r.y+20,24,WHITE);
        DrawText(plans[i].price,(int)r.x+20,(int)r.y+60,18,WHITE);
        string feats=string(plans[i].features);
        int fy=0;
        for(int s=0, e=0; e<=(int)feats.size(); e++){
            if(e==(int)feats.size()||feats[e]=='|'){
                string f=feats.substr(s,e-s);
                while(!f.empty()&&f[0]==' ') f=f.substr(1);
                while(!f.empty()&&f.back()==' ') f.pop_back();
                DrawText(("• "+f).c_str(),(int)r.x+14,(int)r.y+100+fy*28,14,WHITE);
                fy++; s=e+1;
            }
        }
        int ec=0;
        for(int j=0;j<gEnrollCount;j++) ec++;
        DrawText(("Enrollments: "+to_string(i==0?ec:0)).c_str(),(int)r.x+14,(int)r.y+200,13,{220,255,220,255});
    }

    // ── Activate / Cancel Subscription form ──
    DrawPanel({20,375,SW-40,70},COL_ROW_ALT,10.f);
    DrawInputBox("Student ID",{30,400,160,34},inp0,128,0);
    DrawInputBox("Plan (Basic/Pro/Premium)",{200,400,280,34},inp1,128,1);
    if(DrawButton("ACTIVATE",{490,400,120,34},COL_BTN_GREEN,WHITE,13)){
        if(strlen(inp0)<1||strlen(inp1)<2){ setFeedback("Fill Student ID and Plan",false); }
        else {
            EnrollmentRecord e; memset(&e,0,sizeof(e));
            e.enrollmentID=gEnrollCount+1;
            e.studentID=atoi(inp0);
            strncpy(e.courseID,inp1,19);
            strncpy(e.enrolledDate,"2025-01-01",19);
            e.paymentStatus=1; e.isConfirmed=1;
            gEnrollments[gEnrollCount++]=e;
            fh->saveEnrollments(gEnrollments,gEnrollCount);
            fh->logActivity("Admin","ActivateSubscription",inp1);
            clearInputs();
            setFeedback("Subscription activated!",true);
        }
    }
    if(DrawButton("CANCEL",{620,400,100,34},COL_BTN_RED,WHITE,13)){
        if(strlen(inp0)<1){ setFeedback("Enter Student ID to cancel",false); }
        else {
            int sid=atoi(inp0); bool found=false;
            for(int i=0;i<gEnrollCount;i++){
                if(gEnrollments[i].studentID==sid && gEnrollments[i].paymentStatus==1){
                    gEnrollments[i].isConfirmed=0;
                    gEnrollments[i].paymentStatus=0;
                    fh->saveEnrollments(gEnrollments,gEnrollCount);
                    fh->logActivity("Admin","CancelSubscription",inp0);
                    clearInputs();
                    setFeedback("Subscription cancelled!",false);
                    found=true; break;
                }
            }
            if(!found) setFeedback("No active subscription found for this student",false);
        }
    }

    DrawText("All Enrollments", 20, 458, 17, COL_TEXT_DARK);
    DrawRectangle(20,480,SW-40,28,COL_CARD_TEACH);
    DrawText("Enroll ID",30,486,14,WHITE); DrawText("Student",160,486,14,WHITE);
    DrawText("Course/Plan",300,486,14,WHITE); DrawText("Date",500,486,14,WHITE);
    DrawText("Paid",680,486,14,WHITE); DrawText("Confirmed",780,486,14,WHITE);

    HandleScroll(listScroll,(float)max(0,gEnrollCount*30-160));
    BeginScissorMode(20,510,SW-40,150);
    for(int i=0;i<gEnrollCount;i++){
        float y=510+i*30-listScroll;
        if(y<504||y>662) continue;
        Color row=(i%2==0)?WHITE:COL_ROW_ALT;
        DrawRectangle(20,(int)y,SW-40,26,row);
        DrawText(to_string(gEnrollments[i].enrollmentID).c_str(),30,(int)y+6,13,COL_TEXT_DARK);
        DrawText(to_string(gEnrollments[i].studentID).c_str(),160,(int)y+6,13,COL_TEXT_DARK);
        DrawText(gEnrollments[i].courseID,300,(int)y+6,13,COL_TEXT_DARK);
        DrawText(gEnrollments[i].enrolledDate,500,(int)y+6,13,COL_TEXT_DARK);
        DrawText(gEnrollments[i].paymentStatus?"Yes":"No",680,(int)y+6,13,
                 gEnrollments[i].paymentStatus?COL_BTN_GREEN:COL_BTN_RED);
        DrawText(gEnrollments[i].isConfirmed?"Yes":"No",780,(int)y+6,13,
                 gEnrollments[i].isConfirmed?COL_BTN_GREEN:COL_BTN_RED);
    }
    EndScissorMode();
}

// ============================================================
//  ADMIN: MONITOR ENROLLMENTS
// ============================================================
void DrawAdminEnrollments(){
    DrawText("Admin: Monitor Enrollments", 20, 70, 24, COL_ACCENT);

    // stats
    int paid=0, confirmed=0;
    for(int i=0;i<gEnrollCount;i++){
        if(gEnrollments[i].paymentStatus) paid++;
        if(gEnrollments[i].isConfirmed)   confirmed++;
    }
    auto statCard=[&](Rectangle r, Color c, const char* n, const char* v){
        DrawRectangleRounded(r,0.15f,8,c);
        DrawText(n,(int)r.x+12,(int)r.y+10,13,WHITE);
        DrawText(v,(int)r.x+12,(int)r.y+36,26,WHITE);
    };
    statCard({20,100,220,80},COL_BTN_PURPLE,"Total Enrollments",to_string(gEnrollCount).c_str());
    statCard({250,100,220,80},COL_BTN_GREEN,"Paid",to_string(paid).c_str());
    statCard({480,100,220,80},COL_ACCENT,"Confirmed",to_string(confirmed).c_str());

    DrawRectangle(20,196,SW-40,28,COL_ACCENT);
    DrawText("Enroll ID",30,202,14,WHITE); DrawText("Student",160,202,14,WHITE);
    DrawText("Course",310,202,14,WHITE); DrawText("Date",500,202,14,WHITE);
    DrawText("Paid",680,202,14,WHITE); DrawText("Confirmed",790,202,14,WHITE);

    HandleScroll(listScroll,(float)max(0,gEnrollCount*32-430));
    BeginScissorMode(20,226,SW-40,430);
    for(int i=0;i<gEnrollCount;i++){
        float y=226+i*32-listScroll;
        if(y<220||y>660) continue;
        Color row=(i%2==0)?WHITE:COL_ROW_ALT;
        DrawRectangle(20,(int)y,SW-40,28,row);
        DrawText(to_string(gEnrollments[i].enrollmentID).c_str(),30,(int)y+7,13,COL_TEXT_DARK);
        DrawText(to_string(gEnrollments[i].studentID).c_str(),160,(int)y+7,13,COL_TEXT_DARK);
        DrawText(gEnrollments[i].courseID,310,(int)y+7,13,COL_TEXT_DARK);
        DrawText(gEnrollments[i].enrolledDate,500,(int)y+7,13,COL_TEXT_DARK);
        DrawText(gEnrollments[i].paymentStatus?"Paid":"Unpaid",680,(int)y+7,13,
                 gEnrollments[i].paymentStatus?COL_BTN_GREEN:COL_BTN_RED);
        DrawText(gEnrollments[i].isConfirmed?"Yes":"No",790,(int)y+7,13,
                 gEnrollments[i].isConfirmed?COL_BTN_GREEN:COL_BTN_RED);
    }
    EndScissorMode();
}

// ============================================================
//  MAIN  —  The ONE and ONLY main() in the entire project
// ============================================================
int main(){
 SetConfigFlags(FLAG_WINDOW_RESIZABLE);
InitWindow(SW, SH, "SAM  |  E-Learning Platform");
MaximizeWindow();
SetTargetFPS(60);

    fh = new FileHandler("data/");

    // Seed demo data if no users exist
    {
        UserRecord tmp[FH_MAX_USERS];
        int tc = fh->loadUsers(tmp, FH_MAX_USERS);
        if(tc == 0){
            // Admin
            UserRecord u; memset(&u,0,sizeof(u));
            strcpy(u.userID,"ADM_001"); strcpy(u.name,"Admin User");
            strcpy(u.email,"admin@edu.pk"); strcpy(u.password,"admin123");
            strcpy(u.role,"Admin"); u.isActive=1;
            strcpy(u.registrationDate,"2025-01-01");
            fh->registerUser(u);

            // Teacher
            memset(&u,0,sizeof(u));
            strcpy(u.userID,"TCH_001"); strcpy(u.name,"Dr. Sana Ahmed");
            strcpy(u.email,"sana@edu.pk"); strcpy(u.password,"teach99");
            strcpy(u.role,"Teacher"); u.isActive=1;
            strcpy(u.registrationDate,"2025-01-10");
            fh->registerUser(u);

            // Student
            memset(&u,0,sizeof(u));
            strcpy(u.userID,"STU_001"); strcpy(u.name,"Ali Khan");
            strcpy(u.email,"ali@edu.pk"); strcpy(u.password,"pass123");
            strcpy(u.role,"Student"); u.isActive=1;
            strcpy(u.registrationDate,"2025-04-23");
            fh->registerUser(u);

            // Demo courses
            CourseRecord c[2]; memset(c,0,sizeof(c));
            strcpy(c[0].courseID,"C001"); strcpy(c[0].title,"Intro to C++");
            strcpy(c[0].description,"Learn C++ from scratch");
            c[0].price=0; c[0].teacherID=1; c[0].isApproved=1;
            strcpy(c[0].status,"Approved"); strcpy(c[0].type,"Free");
            strcpy(c[1].courseID,"C002"); strcpy(c[1].title,"Data Structures");
            strcpy(c[1].description,"Advanced DS with C++");
            c[1].price=4999; c[1].teacherID=1; c[1].isApproved=0;
            strcpy(c[1].status,"Pending"); strcpy(c[1].type,"Paid");
            strcpy(c[1].subscriptionPlan,"Pro");
            fh->saveCourses(c,2);

            // Demo grade
            GradeRecord g[1]; memset(g,0,sizeof(g));
            strcpy(g[0].courseID,"C001"); g[0].studentID=1; g[0].grade=88.5f;
            fh->saveGrades(g,1);

            // Demo zoom link
            ZoomLinkRecord z[1]; memset(z,0,sizeof(z));
            z[0].linkID=1; strcpy(z[0].teacherName,"Dr. Sana Ahmed");
            strcpy(z[0].section,"BCS-3A"); strcpy(z[0].courseTitle,"Data Structures");
            strcpy(z[0].url,"https://zoom.us/j/123456789");
            strcpy(z[0].scheduledTime,"2025-05-01 10:00");
            fh->saveZoomLinks(z,1);

            // Demo task
            TaskRecord t[2]; memset(t,0,sizeof(t));
            t[0].taskID=1; t[0].studentID=1;
            strcpy(t[0].title,"Complete DS Assignment");
            strcpy(t[0].dueDate,"2025-04-25");
            strcpy(t[0].priority,"High"); t[0].isDone=0;
            t[1].taskID=2; t[1].studentID=1;
            strcpy(t[1].title,"Watch lecture recording");
            strcpy(t[1].dueDate,"2025-04-24");
            strcpy(t[1].priority,"Medium"); t[1].isDone=1;
            fh->saveTasks(t,2);

            // Demo enrollment
            EnrollmentRecord e[1]; memset(e,0,sizeof(e));
            e[0].enrollmentID=1; e[0].studentID=1;
            strcpy(e[0].courseID,"C001");
            strcpy(e[0].enrolledDate,"2025-04-23");
            e[0].paymentStatus=0; e[0].isConfirmed=1;
            fh->saveEnrollments(e,1);
        }
    }
    reloadAll();

    while(!WindowShouldClose()){
        float dt = GetFrameTime();
        if(feedbackTimer > 0) feedbackTimer -= dt;
        if(IsKeyPressed(KEY_ESCAPE)) inputFocus = -1;
        // scroll reset on screen change is handled by navigation buttons

        BeginDrawing();
        ClearBackground(COL_BG);

        // ── TOP BAR ──────────────────────────────────────────
        DrawRectangle(0, 0, SW, 60, COL_TOPBAR);
        DrawText("SAM  |  E-Learning Platform", 20, 16, 30, {255,220,100,255});

        if(!loggedRole.empty()){
            string info = loggedName + "  [" + loggedRole + "]";
            int tw2 = MeasureText(info.c_str(),16);
            DrawText(info.c_str(), SW-tw2-110, 18, 16, {180,220,255,255});
            if(DrawButton("Logout",{(float)(SW-94),12,80,30},{180,60,60,255},WHITE,15)){
                loggedEmail=""; loggedRole=""; loggedName="";
                currentScreen=SCR_HOME;
                clearInputs(); listScroll=0; reloadAll();
            }
        }
        if(DrawButton("Home",{(float)(SW-200),8,90,38},{60,80,160,255},WHITE,20)){
    currentScreen=SCR_HOME; clearInputs(); listScroll=0;
}
        // ─────────────────────────────────────────────────────

        BeginScissorMode(0, 60, SW, SH-52);
auto requireRole = [&](const string& role) -> bool {
        if(loggedRole != role){
            setFeedback("Access denied!", false);
            currentScreen = SCR_LOGIN;
            return false;
        }
        return true;
    };

        switch(currentScreen){
            case SCR_HOME:                  DrawHome();                  break;
            case SCR_LOGIN:                 DrawLogin();                 break;
            case SCR_REGISTER_STUDENT:      DrawRegister(false);         break;
            case SCR_REGISTER_TEACHER:      DrawRegister(true);          break;
            case SCR_AFTER_REGISTER:        DrawAfterRegister();         break;
            case SCR_STUDENT_DASHBOARD:
            if(requireRole("Student")) DrawStudentDashboard();
            break;
        case SCR_TEACHER_DASHBOARD:
            if(requireRole("Teacher")) DrawTeacherDashboard();
            break;
        case SCR_ADMIN_DASHBOARD:
            if(requireRole("Admin")) DrawAdminDashboard();
            break;
            case SCR_GRADEBOOK:             DrawGradebook();             break;
            case SCR_COURSES:               DrawMyCourses();             break;
            case SCR_TASKS:                 DrawTasks();                 break;
            case SCR_ZOOM:                  DrawZoomLinks();             break;
            case SCR_QUIZ:                  DrawQuiz();                  break;
            case SCR_TEACHER_COURSES:       DrawTeacherCourses();        break;
            case SCR_UPLOAD_MATERIAL:       DrawUploadMaterial();        break;
            case SCR_UPLOAD_QUIZ:           DrawUploadQuiz();            break;
            case SCR_VIEW_SUBMISSIONS:      DrawViewSubmissions();       break;
            case SCR_TEACHER_GRADEBOOK:     DrawTeacherGradebook();      break;
            case SCR_ADMIN_APPROVE_COURSES: DrawAdminApproveCourses();   break;
            case SCR_ADMIN_APPROVE_REGS:    DrawAdminApproveRegs();      break;
            case SCR_ADMIN_MANAGE_USERS:    DrawAdminManageUsers();      break;
            case SCR_ADMIN_SUBSCRIPTIONS:   DrawAdminSubscriptions();    break;
            case SCR_ADMIN_ENROLLMENTS:     DrawAdminEnrollments();      break;
            default:                        DrawHome();                  break;
        }

        EndScissorMode();
        DrawFeedback();
        EndDrawing();
    }

    delete fh;
    CloseWindow();
    return 0;
}