#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <time.h> 
 
// Windows specific headers 
#ifdef _WIN32 
    #include <conio.h> 
    #include <windows.h> 
    #define CLEAR "cls" 
    #define SLEEP(x) Sleep(x * 1000) 
#else 
    // Linux/Mac fallback 
    #include <unistd.h> 
    #define CLEAR "clear" 
    #define SLEEP(x) sleep(x) 
#endif 
 
#define MAX_USERS 100 
#define MAX_NAME 50 
#define MAX_PASS 50 
#define QUESTIONS_PER_QUIZ 5 
#define TIME_PER_QUESTION 15 
#define MAX_HISTORY 50 
 
// ============== STRUCTURES ============== 
 
typedef struct { 
    char subject[20]; 
    char difficulty[10]; 
    int score; 
    int totalQuestions; 
    int correctAnswers; 
    char dateTime[30]; 
} QuizHistory; 
 
typedef struct { 
    char name[MAX_NAME]; 
    char password[MAX_PASS]; 
    int totalScore; 
    int quizzesCompleted; 
    int isActive; 
    QuizHistory history[MAX_HISTORY]; 
    int historyCount; 
} User; 
 
typedef struct { 
    char question[256]; 
    char options[4][100]; 
    int correctAnswer; 
} Question; 
 
typedef struct { 
    char name[50]; 
    int numQuestions; 
    int reward; 
} Quiz; 
 
// ============== GLOBAL VARIABLES ============== 
 
User users[MAX_USERS]; 
int userCount = 0; 
int currentUserIndex = -1; 
 
// ============== QUESTION BANKS ============== 
 
Question physicsEasy[] = { 
    {"What is the SI unit of force?", {"Newton", "Joule", "Watt", "Pascal"}, 1}, 
    {"What is the speed of light?", {"3x10^8 m/s", "3x10^6 m/s", "3x10^10 m/s", "3x10^4 m/s"}, 1}, 
    {"Who discovered gravity?", {"Einstein", "Newton", "Galileo", "Hawking"}, 2}, 
    {"What is the unit of current?", {"Volt", "Ohm", "Ampere", "Watt"}, 3}, 
    {"Energy stored in battery is?", {"Kinetic", "Chemical", "Nuclear", "Thermal"}, 2}, 
    {"Formula for speed?", {"Distance/Time", "Time/Distance", "Mass/Volume", "Force/Area"}, 1}, 
    {"Red Planet is?", {"Venus", "Jupiter", "Mars", "Saturn"}, 3}, 
    {"Boiling point of water?", {"50C", "100C", "150C", "200C"}, 2}, 
    {"Sound fastest in?", {"Air", "Water", "Vacuum", "Solid"}, 4}, 
    {"Unit of power?", {"Joule", "Newton", "Watt", "Ampere"}, 3} 
}; 
 
Question physicsMedium[] = { 
    {"Kinetic energy formula?", {"mgh", "1/2 mv^2", "mv", "Fd"}, 2}, 
    {"Ohm's Law?", {"V=IR", "P=VI", "F=ma", "E=mc^2"}, 1}, 
    {"Gravity acceleration on Earth?", {"8.9 m/s^2", "9.8 m/s^2", "10.8 m/s^2", "7.8 m/s^2"}, 2}, 
    {"Newton's 3rd law is about?", {"Inertia", "F=ma", "Action-Reaction", "Gravity"}, 3}, 
    {"Unit of frequency?", {"Second", "Hertz", "Meter", "Joule"}, 2}, 
    {"Momentum formula?", {"ma", "mv", "mgh", "1/2mv^2"}, 2}, 
    {"Rainbow caused by?", {"Reflection", "Refraction", "Diffraction", "Dispersion"}, 4}, 
    {"Absolute zero in Celsius?", {"-273C", "-100C", "0C", "-373C"}, 1}, 
    {"Unit of resistance?", {"Ampere", "Volt", "Ohm", "Watt"}, 3}, 
    {"More inertia?", {"Feather", "Stone", "Paper", "Leaf"}, 2} 
}; 
 
Question physicsHard[] = { 
    {"Planck's constant value?", {"6.626x10^-34", "6.626x10^-32", "6.626x10^-36", 
"6.626x10^-30"}, 1}, 
    {"Gravitational force formula?", {"Gm1m2/r", "Gm1m2/r^2", "Gm1m2/r^3", "Gm1m2*r"}, 2}, 
    {"Escape velocity from Earth?", {"11.2 km/s", "9.8 km/s", "15.2 km/s", "7.8 km/s"}, 1}, 
    {"Photoelectric effect is?", {"e- emission by light", "Light bending", "Sound reflection", "Heat 
absorption"}, 1}, 
    {"Heisenberg principle about?", {"Energy", "Position-momentum", "Mass-energy", "Force"}, 2}, 
    {"Critical angle glass-air?", {"42 deg", "45 deg", "48 deg", "50 deg"}, 1}, 
    {"de Broglie wavelength?", {"h/mv", "h*mv", "mv/h", "h+mv"}, 1}, 
    {"Schwarzschild radius?", {"2GM/c^2", "GM/c^2", "GM/2c^2", "4GM/c^2"}, 1}, 
    {"Fine structure constant?", {"1/137", "1/127", "1/147", "1/157"}, 1}, 
    {"Compton effect causes?", {"Wavelength increase", "Wavelength decrease", "No change", 
"Freq change"}, 1} 
}; 
 
Question mathEasy[] = { 
    {"15 + 27 = ?", {"42", "41", "43", "40"}, 1}, 
    {"Square of 9?", {"72", "81", "90", "63"}, 2}, 
    {"100 / 5 = ?", {"15", "25", "20", "10"}, 3}, 
    {"Value of pi?", {"3.14", "2.14", "4.14", "1.14"}, 1}, 
    {"Hexagon has sides?", {"5", "6", "7", "8"}, 2}, 
    {"7 x 8 = ?", {"54", "55", "56", "57"}, 3}, 
    {"Square root of 64?", {"6", "7", "8", "9"}, 3}, 
    {"50% of 200?", {"50", "75", "100", "150"}, 3}, 
    {"2,4,6,8,?", {"9", "10", "11", "12"}, 2}, 
    {"Perimeter of square side 5?", {"15", "20", "25", "10"}, 2} 
}; 
 
Question mathMedium[] = { 
    {"Derivative of x^2?", {"x", "2x", "x^2", "2"}, 2}, 
    {"sin(90) = ?", {"0", "1", "-1", "0.5"}, 2}, 
    {"log10(100) = ?", {"1", "2", "3", "10"}, 2}, 
    {"Triangle angles sum?", {"90", "180", "270", "360"}, 2}, 
    {"2^10 = ?", {"512", "1024", "2048", "256"}, 2}, 
    {"5! = ?", {"100", "120", "60", "24"}, 2}, 
    {"cos(0) = ?", {"0", "1", "-1", "0.5"}, 2}, 
    {"Integral of 2x?", {"x", "x^2", "2x^2", "x^2+C"}, 4}, 
    {"Euler's number e = ?", {"2.718", "3.14", "1.618", "2.303"}, 1}, 
    {"Area of circle?", {"2*pi*r", "pi*r^2", "pi*d", "2*pi*r^2"}, 2} 
}; 
 
Question mathHard[] = { 
    {"Derivative of ln(x)?", {"x", "1/x", "e^x", "1"}, 2}, 
    {"lim (1+1/n)^n as n->inf?", {"1", "pi", "e", "inf"}, 3}, 
    {"Integral of e^x?", {"e^x", "xe^x", "e^x+C", "ln(x)"}, 3}, 
    {"i^2 = ?", {"1", "-1", "i", "-i"}, 2}, 
    {"det([[1,2],[3,4]])?", {"2", "-2", "10", "-10"}, 2}, 
    {"Sum 1+1/2+1/4+...?", {"1", "2", "inf", "1.5"}, 2}, 
    {"Laplace of 1?", {"1/s", "s", "1", "1/s^2"}, 1}, 
    {"div(curl F) = ?", {"0", "1", "F", "grad F"}, 1}, 
    {"Eigenvalue of I?", {"0", "1", "-1", "inf"}, 2}, 
    {"Taylor e^x at 0?", {"1+x+x^2/2!+..", "1-x+..", "x+x^2+..", "1+x+x^2+.."}, 1} 
}; 
 
Question historyEasy[] = { 
    {"First US President?", {"Lincoln", "Washington", "Jefferson", "Adams"}, 2}, 
    {"WW2 ended in?", {"1943", "1944", "1945", "1946"}, 3}, 
    {"Taj Mahal built by?", {"Akbar", "Shah Jahan", "Jahangir", "Aurangzeb"}, 2}, 
    {"Pyramids built by?", {"Greek", "Roman", "Egyptian", "Persian"}, 3}, 
    {"America discovered by?", {"Vasco da Gama", "Columbus", "Magellan", "Cook"}, 2}, 
    {"India independence?", {"1945", "1946", "1947", "1948"}, 3}, 
    {"Mahatma Gandhi's name?", {"Nehru", "Mohandas", "Bose", "Patel"}, 2}, 
    {"US Civil War between?", {"East-West", "North-South", "US-UK", "US-France"}, 2}, 
    {"First man on moon?", {"Aldrin", "Armstrong", "Gagarin", "Glenn"}, 2}, 
    {"Julius Caesar ruled?", {"Greek", "Persian", "Roman", "Egyptian"}, 3} 
}; 
 
Question historyMedium[] = { 
    {"French Revolution year?", {"1779", "1789", "1799", "1769"}, 2}, 
    {"Declaration of Independence by?", {"Washington", "Adams", "Jefferson", "Franklin"}, 3}, 
    {"Napoleon's final defeat?", {"Austerlitz", "Trafalgar", "Waterloo", "Borodino"}, 3}, 
    {"Berlin Wall built?", {"1951", "1961", "1971", "1981"}, 2}, 
    {"First Emperor of China?", {"Qin Shi Huang", "Han Wudi", "Tang Taizong", "Kublai Khan"}, 1}, 
    {"Russian Revolution?", {"1907", "1917", "1927", "1937"}, 2}, 
    {"Maurya Empire founder?", {"Ashoka", "Bindusara", "Chandragupta", "Chanakya"}, 3}, 
    {"UN formed in?", {"1943", "1944", "1945", "1946"}, 3}, 
    {"Iron Lady of Britain?", {"Victoria", "Elizabeth", "Thatcher", "Diana"}, 3}, 
    {"Cold War ended?", {"1989", "1991", "1993", "1995"}, 2} 
}; 
 
Question historyHard[] = { 
    {"Treaty of Westphalia?", {"1618", "1638", "1648", "1658"}, 3}, 
    {"Last Mughal Emperor?", {"Aurangzeb", "Shah Alam II", "Bahadur Shah II", "Akbar II"}, 3}, 
    {"Magna Carta signed?", {"1115", "1215", "1315", "1415"}, 2}, 
    {"Protestant Reformation led by?", {"Calvin", "Luther", "Zwingli", "Knox"}, 2}, 
    {"Byzantine Empire fell?", {"1353", "1403", "1453", "1503"}, 3}, 
    {"First Caliph of Islam?", {"Umar", "Uthman", "Ali", "Abu Bakr"}, 4}, 
    {"Opium War year?", {"1839-42", "1849-52", "1859-62", "1829-32"}, 1}, 
    {"Gupta Empire founder?", {"Chandragupta I", "Samudragupta", "Chandragupta II", 
"Kumaragupta"}, 1}, 
    {"Congress of Vienna?", {"1805", "1815", "1825", "1835"}, 2}, 
    {"Sun King of France?", {"Louis XIII", "Louis XIV", "Louis XV", "Louis XVI"}, 2} 
}; 
 
Question sportsEasy[] = { 
    {"Football team players?", {"9", "10", "11", "12"}, 3}, 
    {"Cricket invented in?", {"India", "Australia", "England", "South Africa"}, 3}, 
    {"Olympic rings count?", {"4", "5", "6", "7"}, 2}, 
    {"Shuttlecock used in?", {"Tennis", "Badminton", "Squash", "TT"}, 2}, 
    {"Basketball free throw points?", {"1", "2", "3", "4"}, 1}, 
    {"2016 Olympics host?", {"China", "UK", "Brazil", "Japan"}, 3}, 
    {"Cricket team players?", {"9", "10", "11", "12"}, 3}, 
    {"Sachin plays?", {"Football", "Cricket", "Hockey", "Tennis"}, 2}, 
    {"Test cricket ball color?", {"White", "Red", "Pink", "Orange"}, 2}, 
    {"FIFA full form?", {"Football Intl", "Fed Intl Football Assoc", "Football Inst", "None"}, 2} 
}; 
 
Question sportsMedium[] = { 
    {"Most Grand Slams (men)?", {"Federer", "Nadal", "Djokovic", "Sampras"}, 3}, 
    {"Most FIFA World Cups?", {"Germany", "Argentina", "Brazil", "Italy"}, 3}, 
    {"Marathon length (km)?", {"40.195", "42.195", "44.195", "38.195"}, 2}, 
    {"100m world record holder?", {"Bolt", "Lewis", "Gay", "Blake"}, 1}, 
    {"'Love' term in?", {"Badminton", "Tennis", "Squash", "Cricket"}, 2}, 
    {"Grand Slams count?", {"3", "4", "5", "6"}, 2}, 
    {"Basketball invented in?", {"USA", "Canada", "UK", "France"}, 1}, 
    {"Hat-trick means?", {"3 goals", "3 assists", "3 saves", "3 fouls"}, 1}, 
    {"'The Wall' in cricket?", {"Sachin", "Dravid", "Laxman", "Ganguly"}, 2}, 
    {"IPL started in?", {"2006", "2007", "2008", "2009"}, 3} 
}; 
 
Question sportsHard[] = { 
    {"Hand of God goal by?", {"Pele", "Maradona", "Messi", "Ronaldo"}, 2}, 
    {"Highest snooker break?", {"147", "155", "140", "150"}, 1}, 
    {"Most Test centuries?", {"Ponting", "Kallis", "Tendulkar", "Sangakkara"}, 3}, 
    {"First Cricket WC year?", {"1971", "1975", "1979", "1983"}, 2}, 
    {"First FIFA WC winner?", {"Brazil", "Argentina", "Uruguay", "Germany"}, 3}, 
    {"Basketball hoop diameter?", {"16 inch", "17 inch", "18 inch", "19 inch"}, 3}, 
    {"Most F1 championships?", {"Schumacher", "Hamilton", "Vettel", "Prost"}, 2}, 
    {"Shot put weight (men)?", {"6.26 kg", "7.26 kg", "8.26 kg", "5.26 kg"}, 2}, 
    {"First Rugby WC winner?", {"Australia", "New Zealand", "South Africa", "England"}, 2}, 
    {"Olympic pool length?", {"25m", "50m", "75m", "100m"}, 2} 
}; 
 
Question computerEasy[] = { 
    {"CPU stands for?", {"Central Processing Unit", "Computer Personal Unit", "Central Program 
Unit", "None"}, 1}, 
    {"Brain of computer?", {"RAM", "CPU", "Hard Disk", "Monitor"}, 2}, 
    {"Input device?", {"Monitor", "Printer", "Keyboard", "Speaker"}, 3}, 
    {"RAM stands for?", {"Read Access Memory", "Random Access Memory", "Run Access 
Memory", "None"}, 2}, 
    {"Windows made by?", {"Apple", "Google", "Microsoft", "IBM"}, 3}, 
    {"Executable extension?", {".txt", ".exe", ".doc", ".jpg"}, 2}, 
    {"HTML stands for?", {"Hyper Text Markup Lang", "High Text Machine Lang", "None", "All"}, 
1}, 
    {"Volatile memory?", {"ROM", "RAM", "Hard Disk", "SSD"}, 2}, 
    {"1 byte = ?", {"4 bits", "8 bits", "16 bits", "32 bits"}, 2}, 
    {"Output device?", {"Mouse", "Keyboard", "Scanner", "Monitor"}, 4} 
}; 
 
Question computerMedium[] = { 
    {"SQL stands for?", {"Structured Query Lang", "Simple Query Lang", "Standard Query Lang", 
"None"}, 1}, 
    {"Not a programming lang?", {"Java", "Python", "HTML", "C++"}, 3}, 
    {"Binary search complexity?", {"O(n)", "O(log n)", "O(n^2)", "O(1)"}, 2}, 
    {"API stands for?", {"App Programming Interface", "Advanced Program Interface", "None", 
"All"}, 1}, 
    {"LIFO structure?", {"Queue", "Stack", "Array", "Linked List"}, 2}, 
    {"HTTP default port?", {"21", "22", "80", "443"}, 3}, 
    {"CSS stands for?", {"Computer Style Sheets", "Cascading Style Sheets", "None", "All"}, 2}, 
    {"Fastest sorting (avg)?", {"Bubble", "Selection", "Quick", "Insertion"}, 3}, 
    {"Localhost IP?", {"192.168.1.1", "127.0.0.1", "0.0.0.0", "255.255.255.0"}, 2}, 
    {"JSON stands for?", {"JS Object Notation", "Java Standard Object", "None", "All"}, 1} 
}; 
 
Question computerHard[] = { 
    {"Heap sort complexity?", {"O(n)", "O(n log n)", "O(n^2)", "O(log n)"}, 2}, 
    {"Secure browsing protocol?", {"HTTP", "FTP", "HTTPS", "SMTP"}, 3}, 
    {"Deadlock is?", {"System crash", "Circular wait", "Memory overflow", "CPU overload"}, 2}, 
    {"OSI layer 4?", {"Network", "Transport", "Session", "Data Link"}, 2}, 
    {"Polymorphism is?", {"Multiple inherit", "Same interface diff impl", "Data hiding", "Reuse"}, 2}, 
    {"Semaphore is?", {"Data type", "Sync primitive", "Memory type", "Process"}, 2}, 
    {"Quick sort worst case?", {"O(n)", "O(n log n)", "O(n^2)", "O(log n)"}, 3}, 
    {"ACID in DB?", {"Atomicity Consistency Isolation Durability", "None", "All", "Other"}, 1}, 
    {"Race condition is?", {"Fast exec", "Concurrent access issue", "Memory leak", "Stack 
overflow"}, 2}, 
    {"Virtual memory purpose?", {"Increase RAM speed", "Extend physical memory", "Reduce 
CPU", "Increase disk"}, 2} 
}; 
 
Quiz quizzes[5] = { 
    {"Quiz 1", 5, 100}, 
    {"Quiz 2", 5, 150}, 
    {"Quiz 3", 5, 200}, 
    {"Quiz 4", 5, 250}, 
    {"Quiz 5", 5, 300} 
}; 
 
// ============== UTILITY FUNCTIONS ============== 
 
void clearScreen() { 
    system(CLEAR); 
} 
 
void getCurrentDateTime(char* buffer) { 
    time_t now = time(NULL); 
    struct tm* t = localtime(&now); 
    sprintf(buffer, "%02d/%02d/%04d %02d:%02d",  
            t->tm_mday, t->tm_mon + 1, t->tm_year + 1900, 
            t->tm_hour, t->tm_min); 
} 
 
// ============== FILE STORAGE ============== 
 
void saveUsers() { 
    FILE* file = fopen("quiz_data.dat", "wb"); 
    if (file == NULL) { 
        printf("\n   [ERROR: Cannot save!]\n"); 
        return; 
    } 
    fwrite(&userCount, sizeof(int), 1, file); 
    fwrite(users, sizeof(User), userCount, file); 
    fclose(file); 
    printf("\n   [Saved to quiz_data.dat]\n"); 
} 
 
void loadUsers() { 
    FILE* file = fopen("quiz_data.dat", "rb"); 
    if (file != NULL) { 
        fread(&userCount, sizeof(int), 1, file); 
        fread(users, sizeof(User), userCount, file); 
        fclose(file); 
        printf("   [Loaded %d users]\n", userCount); 
    } else { 
        userCount = 0; 
        printf("   [Starting fresh - no data file]\n"); 
    } 
} 
 
// ============== USER MANAGEMENT ============== 
 
int findUser(const char* name) { 
    for (int i = 0; i < userCount; i++) { 
        if (strcmp(users[i].name, name) == 0 && users[i].isActive == 1) { 
            return i; 
        } 
    } 
    return -1; 
} 
 
void addQuizHistory(int userIndex, const char* subject, const char* difficulty,  
                    int score, int total, int correct) { 
    User* user = &users[userIndex]; 
     
    if (user->historyCount >= MAX_HISTORY) { 
        for (int i = 0; i < MAX_HISTORY - 1; i++) { 
            user->history[i] = user->history[i + 1]; 
        } 
        user->historyCount = MAX_HISTORY - 1; 
    } 
     
    QuizHistory* h = &user->history[user->historyCount]; 
    strcpy(h->subject, subject); 
    strcpy(h->difficulty, difficulty); 
    h->score = score; 
    h->totalQuestions = total; 
    h->correctAnswers = correct; 
    getCurrentDateTime(h->dateTime); 
     
    user->historyCount++; 
} 
 
int signup() { 
    clearScreen(); 
    printf("\n"); 
    printf("   
+==============================================================+\n"); 
    printf("   |                      [+] SIGN UP                             |\n"); 
    printf("   
+==============================================================+\n"); 
    printf("   |                                                              |\n"); 
     
    char name[MAX_NAME], password[MAX_PASS], confirmPass[MAX_PASS]; 
     
    printf("   |   [*] Enter username: "); 
    scanf("%s", name); 
     
    if (findUser(name) != -1) { 
        printf("   |                                                              |\n"); 
        printf("   
+==============================================================+\n"); 
        printf("\n   [X] ERROR: Username '%s' already exists!\n", name); 
        return 0; 
    } 
     
    printf("   |   [*] Enter password: "); 
    scanf("%s", password); 
     
    printf("   |   [*] Confirm password: "); 
    scanf("%s", confirmPass); 
     
    if (strcmp(password, confirmPass) != 0) { 
        printf("   |                                                              |\n"); 
        printf("   
+==============================================================+\n"); 
        printf("\n   [X] ERROR: Passwords don't match!\n"); 
        return 0; 
    } 
     
    strcpy(users[userCount].name, name); 
    strcpy(users[userCount].password, password); 
    users[userCount].totalScore = 0; 
    users[userCount].quizzesCompleted = 0; 
    users[userCount].isActive = 1; 
    users[userCount].historyCount = 0; 
    userCount++; 
     
    saveUsers(); 
     
    printf("   |                                                              |\n"); 
    printf("   
+==============================================================+\n"); 
    printf("\n"); 
    printf("   
+==============================================================+\n"); 
    printf("   |            [OK] ACCOUNT CREATED SUCCESSFULLY!                |\n"); 
    printf("   +--------------------------------------------------------------+\n"); 
    printf("   |   [*] Username:  %-42s |\n", name); 
    printf("   |   [*] Score:     %-42d |\n", 0); 
    printf("   |   [*] Quizzes:   %-42d |\n", 0); 
    printf("   
+==============================================================+\n"); 
     
    return 1; 
} 
 
int login() { 
    clearScreen(); 
    printf("\n"); 
    printf("   
+==============================================================+\n"); 
    printf("   |                       [>] LOGIN                              |\n"); 
    printf("   
+==============================================================+\n"); 
    printf("   |                                                              |\n"); 
     
    char name[MAX_NAME], password[MAX_PASS]; 
     
    printf("   |   [*] Enter username: "); 
    scanf("%s", name); 
     
    printf("   |   [*] Enter password: "); 
    scanf("%s", password); 
     
    int idx = findUser(name); 
     
    if (idx == -1) { 
        printf("   |                                                              |\n"); 
        printf("   
+==============================================================+\n"); 
        printf("\n   [X] ERROR: User not found!\n"); 
        return 0; 
    } 
     
    if (strcmp(users[idx].password, password) != 0) { 
        printf("   |                                                              |\n"); 
        printf("   
+==============================================================+\n"); 
        printf("\n   [X] ERROR: Wrong password!\n"); 
        return 0; 
    } 
     
    currentUserIndex = idx; 
    printf("   |                                                              |\n"); 
    printf("   
+==============================================================+\n"); 
    printf("\n   [OK] Welcome back, %s!\n", name); 
    return 1; 
} 
 
void showProfile() { 
    clearScreen(); 
    printf("\n"); 
    printf("   
+==============================================================+\n"); 
    printf("   |                      [*] USER PROFILE                        |\n"); 
    printf("   
+==============================================================+\n"); 
    printf("   |                                                              |\n"); 
     
    User* user = &users[currentUserIndex]; 
    float avg = user->quizzesCompleted > 0 ?  
                (float)user->totalScore / user->quizzesCompleted : 0; 
     
    printf("   |   [*] Name:          %-38s |\n", user->name); 
    printf("   |   [*] Total Score:   %-38d |\n", user->totalScore); 
    printf("   |   [*] Quizzes Done:  %-38d |\n", user->quizzesCompleted); 
    printf("   |   [*] Avg Score:     %-38.1f |\n", avg); 
    printf("   |   [*] History:       %-35d entries |\n", user->historyCount); 
    printf("   |                                                              |\n"); 
    printf("   
+==============================================================+\n"); 
} 
 
void showQuizHistory() { 
    clearScreen(); 
    printf("\n"); 
    printf("   
+==============================================================+\n"); 
    printf("   |                     [#] QUIZ HISTORY                         |\n"); 
    printf("   
+==============================================================+\n"); 
     
    User* user = &users[currentUserIndex]; 
     
    if (user->historyCount == 0) { 
        printf("   |                                                              |\n"); 
        printf("   |         [!] No quiz history yet!                             |\n"); 
        printf("   |         Play some quizzes first.                             |\n"); 
        printf("   |                                                              |\n"); 
        printf("   
+==============================================================+\n"); 
        return; 
    } 
     
    printf("   
+==============================================================+\n"); 
    printf("\n"); 
    printf("   +-----+----------+--------+-------+---------+-----------------+\n"); 
    printf("   | No. | Subject  |  Diff  | Score | Correct |    Date/Time    |\n"); 
    printf("   +-----+----------+--------+-------+---------+-----------------+\n"); 
     
    for (int i = user->historyCount - 1; i >= 0; i--) { 
        QuizHistory* h = &user->history[i]; 
        printf("   | %3d | %-8s | %-6s | %5d |  %d / %d  | %-15s |\n", 
               user->historyCount - i, h->subject, h->difficulty,  
               h->score, h->correctAnswers, h->totalQuestions, h->dateTime); 
    } 
    printf("   +-----+----------+--------+-------+---------+-----------------+\n"); 
     
    int totalCorrect = 0, totalQuestions = 0, totalScore = 0; 
    for (int i = 0; i < user->historyCount; i++) { 
        totalCorrect += user->history[i].correctAnswers; 
        totalQuestions += user->history[i].totalQuestions; 
        totalScore += user->history[i].score; 
    } 
     
    printf("\n"); 
    printf("   
+==============================================================+\n"); 
    printf("   |                      [=] SUMMARY                             |\n"); 
    printf("   +--------------------------------------------------------------+\n"); 
    printf("   |   [#] Total Quizzes:    %-36d |\n", user->historyCount); 
    printf("   |   [?] Total Questions:  %-36d |\n", totalQuestions); 
    printf("   |   [+] Total Correct:    %-36d |\n", totalCorrect); 
    printf("   |   [%%] Overall Accuracy: %-35.1f%% |\n",  
           totalQuestions > 0 ? (float)totalCorrect * 100 / totalQuestions : 0); 
    printf("   |   [$] Total Points:     %-36d |\n", totalScore); 
    printf("   
+==============================================================+\n"); 
} 
 
// ============== TIMED INPUT FOR WINDOWS ============== 
 
#ifdef _WIN32 
int getInputWithTimeout(int timeoutSec) { 
    int timeLeft = timeoutSec; 
    int answer = -1; 
     
    while (timeLeft > 0) { 
        printf("\r   [TIME: %2d sec] Enter answer (1-4): ", timeLeft); 
        fflush(stdout); 
         
        // Check for keypress every 100ms for 1 second 
        for (int i = 0; i < 10; i++) { 
            if (_kbhit()) { 
                char ch = _getch(); 
                if (ch >= '1' && ch <= '4') { 
                    answer = ch - '0'; 
                    printf("%c\n", ch); 
                    return answer; 
                } 
            } 
            Sleep(100); // 100ms 
        } 
         
        timeLeft--; 
    } 
     
    printf("\n\n   [!] TIME'S UP! Moving to next question...\n"); 
    Sleep(1000); 
     
    return -1; 
} 
#else 
// Mac/Linux version with simple timer 
int getInputWithTimeout(int timeoutSec) { 
    time_t start = time(NULL); 
    int answer; 
     
    printf("   [TIME: %d sec] Enter answer (1-4): ", timeoutSec); 
    fflush(stdout); 
     
    scanf("%d", &answer); 
     
    time_t end = time(NULL); 
     
    if ((end - start) > timeoutSec) { 
        printf("\n   [!] TIME'S UP!\n"); 
        return -1; 
    } 
     
    if (answer < 1 || answer > 4) { 
        return -1; 
    } 
     
    return answer; 
} 
#endif 
 
// ============== QUIZ FUNCTIONS ============== 
 
Question* getQuestionBank(int subject, int difficulty) { 
    switch (subject) { 
        case 1: 
            switch (difficulty) { 
                case 1: return physicsEasy; 
                case 2: return physicsMedium; 
                case 3: return physicsHard; 
            } 
            break; 
        case 2: 
            switch (difficulty) { 
                case 1: return mathEasy; 
                case 2: return mathMedium; 
                case 3: return mathHard; 
            } 
            break; 
        case 3: 
            switch (difficulty) { 
                case 1: return historyEasy; 
                case 2: return historyMedium; 
                case 3: return historyHard; 
            } 
            break; 
        case 4: 
            switch (difficulty) { 
                case 1: return sportsEasy; 
                case 2: return sportsMedium; 
                case 3: return sportsHard; 
            } 
            break; 
        case 5: 
            switch (difficulty) { 
                case 1: return computerEasy; 
                case 2: return computerMedium; 
                case 3: return computerHard; 
            } 
            break; 
    } 
    return NULL; 
} 
 
void shuffleIndices(int* arr, int n) { 
    srand((unsigned int)time(NULL)); 
    for (int i = n - 1; i > 0; i--) { 
        int j = rand() % (i + 1); 
        int temp = arr[i]; 
        arr[i] = arr[j]; 
        arr[j] = temp; 
    } 
} 
 
int conductQuiz(int subject, int difficulty, int quizIndex) { 
    Question* bank = getQuestionBank(subject, difficulty); 
    if (bank == NULL) return 0; 
     
    const char* subjects[] = {"", "Physics", "Math", "History", "Sports", "Computer"}; 
    const char* diffs[] = {"", "Easy", "Medium", "Hard"}; 
     
    clearScreen(); 
    printf("\n"); 
    printf("   
+==============================================================+\n"); 
    printf("   |                    [>] QUIZ STARTING                         |\n"); 
    printf("   
+==============================================================+\n"); 
    printf("   |                                                              |\n"); 
    printf("   |   [#] Subject:      %-40s |\n", subjects[subject]); 
    printf("   |   [!] Difficulty:   %-40s |\n", diffs[difficulty]); 
    printf("   |   [?] Questions:    %-40d |\n", QUESTIONS_PER_QUIZ); 
    printf("   |   [@] Time/Question: %-37d sec |\n", TIME_PER_QUESTION); 
    printf("   |   [$] Reward:       %-40d |\n", quizzes[quizIndex].reward); 
    printf("   |                                                              |\n"); 
    printf("   +--------------------------------------------------------------+\n"); 
    printf("   |                                                              |\n"); 
    printf("   |   [!] Just press 1/2/3/4 to answer (no Enter needed)         |\n"); 
    printf("   |   [!] Time up = Auto skip to next question                   |\n"); 
    printf("   |                                                              |\n"); 
    printf("   
+==============================================================+\n"); 
     
    printf("\n   Press ENTER to start quiz..."); 
    getchar(); 
    getchar(); 
     
    int indices[10] = {0,1,2,3,4,5,6,7,8,9}; 
    shuffleIndices(indices, 10); 
     
    int correct = 0; 
    int skipped = 0; 
     
    for (int i = 0; i < QUESTIONS_PER_QUIZ; i++) { 
        clearScreen(); 
         
        Question* q = &bank[indices[i]]; 
         
        printf("\n"); 
        printf("   
+==============================================================+\n"); 
        printf("   |   [?] Question %d of %d                      [TIME: %d sec]   |\n",  
               i + 1, QUESTIONS_PER_QUIZ, TIME_PER_QUESTION); 
        printf("   
+==============================================================+\n"); 
        printf("   |                                                              |\n"); 
        printf("   |   Q: %-56s |\n", q->question); 
        printf("   |                                                              |\n"); 
        printf("   |      [1] %-52s |\n", q->options[0]); 
        printf("   |      [2] %-52s |\n", q->options[1]); 
        printf("   |      [3] %-52s |\n", q->options[2]); 
        printf("   |      [4] %-52s |\n", q->options[3]); 
        printf("   |                                                              |\n"); 
        printf("   
+==============================================================+\n\n"); 
         
        int answer = getInputWithTimeout(TIME_PER_QUESTION); 
         
        if (answer == -1) { 
            printf("\n   [i] Correct answer: %d. %s\n",  
                   q->correctAnswer, q->options[q->correctAnswer - 1]); 
            skipped++; 
        } 
        else if (answer == q->correctAnswer) { 
            printf("\n   [+] CORRECT! Well done!\n"); 
            correct++; 
        } else { 
            printf("\n   [X] WRONG! Correct answer: %d. %s\n",  
                   q->correctAnswer, q->options[q->correctAnswer - 1]); 
        } 
         
        printf("\n   [=] Score: %d | Skipped: %d\n", correct, skipped); 
        SLEEP(2); 
    } 
     
    int points = (correct * quizzes[quizIndex].reward) / QUESTIONS_PER_QUIZ; 
     
    clearScreen(); 
    printf("\n"); 
    printf("   
+==============================================================+\n"); 
    printf("   |                                                              |\n"); 
    printf("   |                  [*] QUIZ COMPLETED! [*]                     |\n"); 
    printf("   |                                                              |\n"); 
    printf("   
+==============================================================+\n"); 
    printf("   |                                                              |\n"); 
    printf("   |   [+] Correct:    %d / %d                                     |\n", correct, 
QUESTIONS_PER_QUIZ); 
    printf("   |   [X] Wrong:      %d                                          |\n", QUESTIONS_PER_QUIZ - 
correct - skipped); 
    printf("   |   [>] Skipped:    %d (time up)                                |\n", skipped); 
    printf("   |   [%%] Accuracy:   %.0f%%                                       |\n", (float)correct * 100 / 
QUESTIONS_PER_QUIZ); 
    printf("   |   [$] Points:     %d                                         |\n", points); 
    printf("   |                                                              |\n"); 
    printf("   
+==============================================================+\n"); 
     
    users[currentUserIndex].totalScore += points; 
    users[currentUserIndex].quizzesCompleted++; 
     
    addQuizHistory(currentUserIndex, subjects[subject], diffs[difficulty], 
                   points, QUESTIONS_PER_QUIZ, correct); 
     
    saveUsers(); 
     
    printf("\n   [OK] Progress saved!\n"); 
     
    return points; 
} 
 
int selectSubject() { 
    clearScreen(); 
    printf("\n"); 
    printf("   
+==============================================================+\n"); 
    printf("   |                   [#] SELECT SUBJECT                         |\n"); 
    printf("   
+==============================================================+\n"); 
    printf("   |                                                              |\n"); 
    printf("   |         1. [~] Physics                                       |\n"); 
    printf("   |         2. [+] Mathematics                                   |\n"); 
    printf("   |         3. [#] History                                       |\n"); 
    printf("   |         4. [o] Sports                                        |\n"); 
    printf("   |         5. [@] Computer                                      |\n"); 
    printf("   |         0. [<] Back                                          |\n"); 
    printf("   |                                                              |\n"); 
    printf("   
+==============================================================+\n"); 
    printf("\n   Enter choice: "); 
    int c; scanf("%d", &c); 
    return c; 
} 
 
int selectDifficulty() { 
    clearScreen(); 
    printf("\n"); 
    printf("   
+==============================================================+\n"); 
    printf("   |                   [!] SELECT DIFFICULTY                      |\n"); 
    printf("   
+==============================================================+\n"); 
    printf("   |                                                              |\n"); 
    printf("   |         1. [E] Easy   - For beginners                        |\n"); 
    printf("   |         2. [M] Medium - For intermediate                     |\n"); 
    printf("   |         3. [H] Hard   - For experts                          |\n"); 
    printf("   |         0. [<] Back                                          |\n"); 
    printf("   |                                                              |\n"); 
    printf("   
+==============================================================+\n"); 
    printf("\n   Enter choice: "); 
    int c; scanf("%d", &c); 
    return c; 
} 
 
int selectQuiz() { 
    clearScreen(); 
    printf("\n"); 
    printf("   
+==============================================================+\n"); 
    printf("   |                     [>] SELECT QUIZ                          |\n"); 
    printf("   
+==============================================================+\n"); 
    printf("   |                                                              |\n"); 
    printf("   |   +-----+--------------+------------+----------+             |\n"); 
    printf("   |   | No. |  Quiz Name   |  Questions |  Reward  |             |\n"); 
    printf("   |   +-----+--------------+------------+----------+             |\n"); 
    printf("   |   |  1  |  Quiz 1      |      5     |   100 $  |             |\n"); 
    printf("   |   |  2  |  Quiz 2      |      5     |   150 $  |             |\n"); 
    printf("   |   |  3  |  Quiz 3      |      5     |   200 $  |             |\n"); 
    printf("   |   |  4  |  Quiz 4      |      5     |   250 $  |             |\n"); 
    printf("   |   |  5  |  Quiz 5      |      5     |   300 $  |             |\n"); 
    printf("   |   +-----+--------------+------------+----------+             |\n"); 
    printf("   |                                                              |\n"); 
    printf("   |         0. [<] Back                                          |\n"); 
    printf("   |                                                              |\n"); 
    printf("   
+==============================================================+\n"); 
    printf("\n   Enter quiz number: "); 
    int c; scanf("%d", &c); 
    return c; 
} 
 
void mainMenu() { 
    int choice; 
     
    do { 
        clearScreen(); 
        printf("\n"); 
        printf("   
+==============================================================+\n"); 
        printf("   |                    [=] MAIN MENU                             |\n"); 
        printf("   
+==============================================================+\n"); 
        printf("   |                                                              |\n"); 
        printf("   |   [*] User: %-20s                           |\n", users[currentUserIndex].name); 
        printf("   |   [$] Score: %-19d                           |\n", users[currentUserIndex].totalScore); 
        printf("   |   [#] Quizzes: %-17d                           |\n", 
users[currentUserIndex].quizzesCompleted); 
        printf("   |                                                              |\n"); 
        printf("   +--------------------------------------------------------------+\n"); 
        printf("   |                                                              |\n"); 
        printf("   |         1. [>] Start Quiz                                    |\n"); 
        printf("   |         2. [*] View Profile                                  |\n"); 
        printf("   |         3. [#] Quiz History                                  |\n"); 
        printf("   |         4. [X] Logout                                        |\n"); 
        printf("   |                                                              |\n"); 
        printf("   
+==============================================================+\n"); 
        printf("\n   Enter choice: "); 
        scanf("%d", &choice); 
         
        switch (choice) { 
            case 1: { 
                int subj = selectSubject(); 
                if (subj >= 1 && subj <= 5) { 
                    int diff = selectDifficulty(); 
                    if (diff >= 1 && diff <= 3) { 
                        int quiz = selectQuiz(); 
                        if (quiz >= 1 && quiz <= 5) { 
                            conductQuiz(subj, diff, quiz - 1); 
                            printf("\n   Press ENTER..."); 
                            getchar(); getchar(); 
                        } 
                    } 
                } 
                break; 
            } 
            case 2: 
                showProfile(); 
                printf("\n   Press ENTER..."); 
                getchar(); getchar(); 
                break; 
            case 3: 
                showQuizHistory(); 
                printf("\n   Press ENTER..."); 
                getchar(); getchar(); 
                break; 
            case 4: 
                printf("\n   Goodbye, %s!\n", users[currentUserIndex].name); 
                currentUserIndex = -1; 
                SLEEP(1); 
                break; 
        } 
    } while (choice != 4); 
} 
 
void welcomeScreen() { 
    int choice; 
     
    do { 
        clearScreen(); 
        printf("\n"); 
        printf("   
+==============================================================+\n"); 
        printf("   |                                                              |\n"); 
        printf("   |     QQQQQ   U   U   III   ZZZZZ                              |\n"); 
        printf("   |    Q     Q  U   U    I       Z                               |\n"); 
        printf("   |    Q     Q  U   U    I      Z                                |\n"); 
        printf("   |    Q  Q  Q  U   U    I     Z                                 |\n"); 
        printf("   |     QQQQQ    UUU    III   ZZZZZ                              |\n"); 
        printf("   |                                                              |\n"); 
        printf("   |                  QUIZ MASTER                                 |\n"); 
        printf("   |              B.Tech 1st Year Project                         |\n"); 
        printf("   |                                                              |\n"); 
        printf("   
+==============================================================+\n"); 
        printf("   |                                                              |\n"); 
        printf("   |         1. [>] Login                                         |\n"); 
        printf("   |         2. [+] Sign Up                                       |\n"); 
        printf("   |         3. [X] Exit                                          |\n"); 
        printf("   |                                                              |\n"); 
        printf("   
+==============================================================+\n"); 
        printf("\n   Enter choice: "); 
        scanf("%d", &choice); 
         
        switch (choice) { 
            case 1: 
                if (login()) { 
                    printf("\n   Press ENTER..."); 
                    getchar(); getchar(); 
                    mainMenu(); 
                } else { 
                    printf("\n   Press ENTER..."); 
                    getchar(); getchar(); 
                } 
                break; 
            case 2: 
                signup(); 
                printf("\n   Press ENTER..."); 
                getchar(); getchar(); 
                break; 
            case 3: 
                clearScreen(); 
                printf("\n"); 
                printf("   
+==============================================================+\n"); 
                printf("   |                                                              |\n"); 
                printf("   |           Thank you for using Quiz Master!                   |\n"); 
                printf("   |                   See you next time!                         |\n"); 
                printf("   |                                                              |\n"); 
                printf("   
+==============================================================+\n\n"); 
                exit(0); 
        } 
    } while (1); 
} 
 
int main() { 
    clearScreen(); 
    printf("\n"); 
    printf("   
+==============================================================+\n"); 
    printf("   |                                                              |\n"); 
    printf("   |              QUIZ MASTER - Loading...                        |\n");
    printf("   |                                                              |\n"); 
    printf("   
+==============================================================+\n\n"); 
     
    loadUsers(); 
     
    printf("\n   Press ENTER to continue..."); 
    getchar(); 
    welcomeScreen(); 
    return 0; 
}
