#define CAT1_NAME "_Shaiik0"
#define CAT1_CODE "482917"
#define CAT2_NAME "Toza"
#define CAT2_CODE "731506"
#define CAT3_NAME "ShiroOnMc"
#define CAT3_CODE "295843"

#define GH_USER   "ShinraOnTop"
#define GH_REPO   "shinra-music"
#define GH_DIR    "musique"
#define GH_BRANCH "main"
#define GH_SRC    "ShinraMusic.c"
#define GH_BG     "fond.bmp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CAT1_NAME "_Shaiik0"
#define CAT1_CODE "482917"
#define CAT2_NAME "Toza"
#define CAT2_CODE "731506"
#define CAT3_NAME "ShiroOnMc"
#define CAT3_CODE "295843"

#define GH_USER   "ShinraOnTop"
#define GH_REPO   "shinra-music"
#define GH_DIR    "musique"
#define GH_BRANCH "main"
#define GH_SRC    "ShinraMusic.c"
#define GH_BG     "fond.bmp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef IDC_HAND
#define IDC_HAND MAKEINTRESOURCE(32649)
#endif
#ifndef WM_APP
#define WM_APP 0x8000
#endif
#ifndef CREATE_NO_WINDOW
#define CREATE_NO_WINDOW 0x08000000
#endif
#ifndef HANDLE_FLAG_INHERIT
#define HANDLE_FLAG_INHERIT 1
#endif
#ifndef STARTF_USESHOWWINDOW
#define STARTF_USESHOWWINDOW 0x00000001
#endif
#ifndef STARTF_USESTDHANDLES
#define STARTF_USESTDHANDLES 0x00000100
#endif
#ifndef WAIT_OBJECT_0
#define WAIT_OBJECT_0 0
#endif
#ifndef CP_ACP
#define CP_ACP 0
#endif
#ifndef CP_UTF8
#define CP_UTF8 65001
#endif

#ifndef IMAGE_ICON
#define IMAGE_ICON 1
#endif
#ifndef LR_LOADFROMFILE
#define LR_LOADFROMFILE 0x00000010
#endif
#ifndef WM_SETICON
#define WM_SETICON 0x0080
#endif
#ifndef ICON_SMALL
#define ICON_SMALL 0
#endif
#ifndef ICON_BIG
#define ICON_BIG 1
#endif
#ifndef SM_CXICON
#define SM_CXICON 11
#define SM_CYICON 12
#endif
#ifndef SM_CXSMICON
#define SM_CXSMICON 49
#define SM_CYSMICON 50
#endif

static HICON icoBig = NULL, icoSm = NULL;

#ifdef __TINYC__
int WINAPI MultiByteToWideChar(UINT, DWORD, const char *, int, unsigned short *, int);
int WINAPI WideCharToMultiByte(UINT, DWORD, const unsigned short *, int, char *, int, const char *, int *);
#endif

#define WIN_W    960
#define WIN_H    620
#define TITLE_H  34
#define CODE_MAX 12
#define MAX_CAT  6
#define MAX_TR   2048
#define ROWH     36
#define VIS      9

#define WM_DL_DONE  (WM_APP + 1)
#define WM_PL_DONE  (WM_APP + 2)
#define WM_CFG_DONE (WM_APP + 3)
#define WM_BG_DONE  (WM_APP + 4)
#define CLIP_TEXT   1
#define WM_UPD_READY (WM_APP + 5)

#ifndef APP_BUILD
#define APP_BUILD 0
#endif
#ifndef MOVEFILE_REPLACE_EXISTING
#define MOVEFILE_REPLACE_EXISTING 0x00000001
#endif
#ifndef MOVEFILE_COPY_ALLOWED
#define MOVEFILE_COPY_ALLOWED 0x00000002
#endif

#ifndef VK_INSERT
#define VK_INSERT 0x2D
#endif

typedef DWORD (WINAPI *MCIFN)(const char *, char *, UINT, void *);

static MCIFN pMci = NULL;

enum { SCR_CATS, SCR_CODE, SCR_LIST };
enum { H_NONE = -1, H_PREV = 5000, H_PLAY, H_NEXT, H_STOP, H_SEEK, H_VOL,
       H_SHUF, H_REP, H_MIN, H_CLOSE, H_BACK };

typedef struct { char name[160]; char url[512]; } Track;
typedef struct { char url[512]; char path[MAX_PATH]; int gen; } DlArg;
typedef struct { char name[48]; char code[CODE_MAX + 1]; } Cat;

static HWND    hwnd;
static int     screen = SCR_CATS;
static int     hot = H_NONE, drag = H_NONE;
static HBITMAP bg = NULL;
static int     bgW = 0, bgH = 0;
static HFONT   fTitle, fBig, fMid, fMidB, fSm, fMono;
static COLORREF accent;
static char    bgTmp[MAX_PATH] = "";
static char    selfPath[MAX_PATH] = "";
static char    updTmp[MAX_PATH] = "";
static int     updPending = 0;

static char    codeInput[CODE_MAX + 2];
static int     codeLen = 0;
static char    info[160] = "";

static Cat     cats[MAX_CAT];
static Cat     cfgCats[MAX_CAT];
static int     ncat = 0, cfgN = 0, selCat = 0;
static int     catTries[MAX_CAT], catLocked[MAX_CAT];
static char    catName[48] = "";
static volatile int plBusy = 0;
static char    cfgBuf[262144];

static RECT    btn[MAX_CAT + 2];
static int     nbtn = 0;
static RECT    rList, rPlayer, rSeek, rVol, rPrev, rPlay, rNext, rStop, rShuf, rRep;
static RECT    rNow, rMin, rClose, rBack;

static Track   tracks[MAX_TR];
static int     ntracks = 0, scroll = 0, cur = -1;
static int     tn = 0;
static volatile int plState = 0;
static char    plErr[160] = "";
static char    netBuf[1000000];

static int     isOpen = 0, playing = 0, paused = 0, volume = 700;
static int     shuffle = 0, repeat = 0;
static volatile int loading = 0;
static volatile int reqGen = 0;
static char    playErr[96] = "";
static char    tmpPath[MAX_PATH] = "";

static LONG WINAPI crashHandler(EXCEPTION_POINTERS *ep)
{
    char b[160];
    sprintf(b, "PLANTAGE code=%08lX adresse=%p",
            (unsigned long)ep->ExceptionRecord->ExceptionCode,
            ep->ExceptionRecord->ExceptionAddress);
    MessageBoxA(NULL, b, "ShinraMusic - plantage", MB_OK | MB_ICONERROR);
    return EXCEPTION_EXECUTE_HANDLER;
}

static int runCapture(const char *cmdline, char *out, int outsz)
{
    SECURITY_ATTRIBUTES sa;
    HANDLE rd = NULL, wr = NULL;
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    char cmd[3000], scratch[256];
    DWORD got = 0, code = 1;
    int n = 0;

    sa.nLength = sizeof sa;
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    if (!CreatePipe(&rd, &wr, &sa, 0)) return -1;
    SetHandleInformation(rd, HANDLE_FLAG_INHERIT, 0);

    memset(&si, 0, sizeof si);
    si.cb = sizeof si;
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = wr;
    si.hStdError = wr;
    si.hStdInput = NULL;

    strncpy(cmd, cmdline, sizeof cmd - 1);
    cmd[sizeof cmd - 1] = 0;
    memset(&pi, 0, sizeof pi);
    if (!CreateProcessA(NULL, cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        CloseHandle(rd); CloseHandle(wr);
        return -1;
    }
    CloseHandle(wr);

    if (out && outsz > 1) {
        while (n < outsz - 1 && ReadFile(rd, out + n, (DWORD)(outsz - 1 - n), &got, NULL) && got > 0)
            n += (int)got;
        out[n] = 0;
    } else {
        while (ReadFile(rd, scratch, sizeof scratch, &got, NULL) && got > 0) { }
    }
    CloseHandle(rd);

    if (WaitForSingleObject(pi.hProcess, 8000) != WAIT_OBJECT_0) TerminateProcess(pi.hProcess, 1);
    GetExitCodeProcess(pi.hProcess, &code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return (int)code;
}

static void utf8ToAnsi(const char *s, char *out, int outsz)
{
    char src[300];
    unsigned short w[320];
    strncpy(src, s, sizeof src - 1);
    src[sizeof src - 1] = 0;
    if (MultiByteToWideChar(CP_UTF8, 0, src, -1, w, 320) > 0 &&
        WideCharToMultiByte(CP_ACP, 0, w, -1, out, outsz, NULL, NULL) > 0)
        return;
    strncpy(out, src, outsz - 1);
    out[outsz - 1] = 0;
}

static int hexv(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static void urlEnc(const char *s, char *out, int outsz)
{
    int n = 0;
    for (; *s && n < outsz - 4; s++) {
        unsigned char c = (unsigned char)*s;
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
            c == '-' || c == '_' || c == '.' || c == '~' || c == '/')
            out[n++] = (char)c;
        else
            n += sprintf(out + n, "%%%02X", c);
    }
    out[n] = 0;
}

static void jsonStrU8(const char *s, char *out, int outsz)
{
    int n = 0;
    while (*s && *s != '"' && n < outsz - 4) {
        if (*s == '\\') {
            s++;
            if (*s == 'u') {
                unsigned cp = 0;
                int k, bad = 0;
                for (k = 1; k <= 4; k++) {
                    int v = hexv(s[k]);
                    if (v < 0) { bad = 1; break; }
                    cp = (cp << 4) | (unsigned)v;
                }
                if (bad) { s++; continue; }
                s += 5;
                if (cp >= 0xD800 && cp <= 0xDFFF) continue;
                if (cp < 0x80) out[n++] = (char)cp;
                else if (cp < 0x800) {
                    out[n++] = (char)(0xC0 | (cp >> 6));
                    out[n++] = (char)(0x80 | (cp & 63));
                } else {
                    out[n++] = (char)(0xE0 | (cp >> 12));
                    out[n++] = (char)(0x80 | ((cp >> 6) & 63));
                    out[n++] = (char)(0x80 | (cp & 63));
                }
                continue;
            }
            if (*s == 0) break;
            out[n++] = *s++;
        } else {
            out[n++] = *s++;
        }
    }
    out[n] = 0;
}

static void compactJson(char *s)
{
    char *w = s;
    int inStr = 0;
    for (; *s; s++) {
        if (inStr) {
            *w++ = *s;
            if (*s == '\\' && s[1]) *w++ = *++s;
            else if (*s == '"') inStr = 0;
        } else if (*s == '"') {
            inStr = 1;
            *w++ = *s;
        } else if (*s != ' ' && *s != '\n' && *s != '\r' && *s != '\t') {
            *w++ = *s;
        }
    }
    *w = 0;
}

static void parseGithub(char *buf)
{
    char *pos = buf;

    while (tn < MAX_TR && (pos = strstr(pos, "\"name\":\"")) != NULL) {
        char *nm = pos + 8, *u, *nx, *dot;
        char nameU8[300], url[512];
        Track *t;

        pos = nm;
        u = strstr(nm, "\"download_url\":\"");
        if (!u) break;
        nx = strstr(nm, "\"name\":\"");
        if (nx && nx < u) continue;
        jsonStrU8(nm, nameU8, sizeof nameU8);
        jsonStrU8(u + 16, url, sizeof url);
        dot = strrchr(nameU8, '.');
        if (!dot || !(!lstrcmpiA(dot, ".mp3") || !lstrcmpiA(dot, ".wav") ||
                      !lstrcmpiA(dot, ".m4a") || !lstrcmpiA(dot, ".wma")))
            continue;
        *dot = 0;
        if (strncmp(url, "http", 4) || strchr(url, '"') || strchr(url, ' ')) continue;
        t = &tracks[tn];
        utf8ToAnsi(nameU8, t->name, sizeof t->name);
        if (!t->name[0]) sprintf(t->name, "Titre %d", tn + 1);
        strcpy(t->url, url);
        tn++;
    }
}

static int cfgGet(const char *buf, int n, const char *field, char *out, int outsz)
{
    char key[48];
    const char *p, *e;
    int i, len;

    sprintf(key, "#define CAT%d_%s \"", n, field);
    p = strstr(buf, key);
    if (!p) return 0;
    p += strlen(key);
    e = strchr(p, '"');
    if (!e) return 0;
    len = (int)(e - p);
    if (len <= 0 || len >= outsz) return 0;
    for (i = 0; i < len; i++) if ((unsigned char)p[i] < 32) return 0;
    memcpy(out, p, len);
    out[len] = 0;
    return 1;
}

static DWORD WINAPI cfgThread(LPVOID unused)
{
    char cmd[500];
    int n;
    (void)unused;

    cfgN = 0;
    sprintf(cmd, "curl.exe -s -L -m 15 \"https://raw.githubusercontent.com/%s/%s/%s/%s\"",
            GH_USER, GH_REPO, GH_BRANCH, GH_SRC);
    if (runCapture(cmd, cfgBuf, sizeof cfgBuf) == 0 && cfgBuf[0]) {
        for (n = 1; n <= MAX_CAT; n++) {
            Cat *c = &cfgCats[cfgN];
            if (!cfgGet(cfgBuf, n, "NAME", c->name, sizeof c->name)) break;
            if (!cfgGet(cfgBuf, n, "CODE", c->code, sizeof c->code)) break;
            cfgN++;
        }
    }
    PostMessageA(hwnd, WM_CFG_DONE, 0, 0);
    return 0;
}

static DWORD WINAPI bgThread(LPVOID unused)
{
    char cmd[900];
    int rc;
    (void)unused;

    sprintf(cmd, "curl.exe -s -L -m 60 -o \"%s\" \"https://raw.githubusercontent.com/%s/%s/%s/%s\"",
            bgTmp, GH_USER, GH_REPO, GH_BRANCH, GH_BG);
    rc = runCapture(cmd, NULL, 0);
    PostMessageA(hwnd, WM_BG_DONE, (WPARAM)(rc == 0), 0);
    return 0;
}

static DWORD WINAPI updThread(LPVOID unused)
{
    char cmd[900], out[64], oldp[MAX_PATH + 8];
    int remote;
    FILE *f;
    long sz = 0;
    unsigned char hdr[2] = { 0, 0 };
    (void)unused;

    sprintf(oldp, "%s.old", selfPath);
    DeleteFileA(oldp);

    sprintf(cmd, "curl.exe -s -L -m 15 \"https://github.com/%s/%s/releases/latest/download/version.txt\"",
            GH_USER, GH_REPO);
    out[0] = 0;
    if (runCapture(cmd, out, sizeof out) != 0 || !out[0]) return 0;
    remote = atoi(out);
    if (remote <= APP_BUILD) return 0;

    sprintf(cmd, "curl.exe -s -L -m 120 -o \"%s\" \"https://github.com/%s/%s/releases/latest/download/ShinraMusic.exe\"",
            updTmp, GH_USER, GH_REPO);
    if (runCapture(cmd, NULL, 0) != 0) { remove(updTmp); return 0; }

    f = fopen(updTmp, "rb");
    if (f) {
        if (fread(hdr, 1, 2, f) != 2) hdr[0] = 0;
        fseek(f, 0, SEEK_END);
        sz = ftell(f);
        fclose(f);
    }
    if (sz < 100000 || hdr[0] != 'M' || hdr[1] != 'Z') { remove(updTmp); return 0; }

    PostMessageA(hwnd, WM_UPD_READY, 0, 0);
    return 0;
}

static DWORD WINAPI plThread(LPVOID unused)
{
    char cmd[1500], raw[200], enc[600];
    (void)unused;

    tn = 0;
    plErr[0] = 0;
    if (strstr(GH_USER, "TONPSEUDO")) {
        strcpy(plErr, "Renseigne GH_USER et GH_REPO en haut de ShinraMusic.c");
        goto done;
    }
    if (GH_DIR[0]) sprintf(raw, "%s/%s", GH_DIR, catName);
    else           strcpy(raw, catName);
    urlEnc(raw, enc, sizeof enc);
    sprintf(cmd, "curl.exe -s -L -m 25 -H \"User-Agent: ShinraMusic\" "
                 "-H \"Accept: application/vnd.github+json\" "
                 "\"https://api.github.com/repos/%s/%s/contents/%s\"",
            GH_USER, GH_REPO, enc);
    if (runCapture(cmd, netBuf, sizeof netBuf) != 0 || !netBuf[0]) {
        strcpy(plErr, "GitHub injoignable (internet ?)");
        goto done;
    }
    compactJson(netBuf);
    parseGithub(netBuf);
    if (tn == 0) {
        if (strstr(netBuf, "rate limit"))
            strcpy(plErr, "Limite GitHub atteinte (r\xe9" "essaie dans un moment)");
        else if (strstr(netBuf, "\"message\""))
            strcpy(plErr, "D\xe9p\xf4t ou dossier introuvable (d\xe9p\xf4t public ? noms exacts ?)");
        else
            strcpy(plErr, "Aucune musique (mp3, wav, m4a, wma) dans ce dossier");
    }

done:
    ntracks = tn;
    plState = tn > 0 ? 1 : 2;
    plBusy = 0;
    PostMessageA(hwnd, WM_PL_DONE, (WPARAM)tn, 0);
    return 0;
}

static DWORD WINAPI dlThread(LPVOID p)
{
    DlArg *a = (DlArg *)p;
    char cmd[1400];
    int rc;

    sprintf(cmd, "curl.exe -s -L -m 180 -o \"%s\" \"%s\"", a->path, a->url);
    rc = runCapture(cmd, NULL, 0);
    if (a->gen != reqGen) {
        remove(a->path);
        free(a);
        return 0;
    }
    PostMessageA(hwnd, WM_DL_DONE, (WPARAM)(rc == 0), (LPARAM)a->gen);
    free(a);
    return 0;
}

static void addCat(const char *name, const char *code)
{
    if (ncat >= MAX_CAT) return;
    strncpy(cats[ncat].name, name, sizeof cats[ncat].name - 1);
    cats[ncat].name[sizeof cats[ncat].name - 1] = 0;
    strncpy(cats[ncat].code, code, CODE_MAX);
    cats[ncat].code[CODE_MAX] = 0;
    ncat++;
}

static void initCats(void)
{
    ncat = 0;
#ifdef CAT1_NAME
    addCat(CAT1_NAME, CAT1_CODE);
#endif
#ifdef CAT2_NAME
    addCat(CAT2_NAME, CAT2_CODE);
#endif
#ifdef CAT3_NAME
    addCat(CAT3_NAME, CAT3_CODE);
#endif
#ifdef CAT4_NAME
    addCat(CAT4_NAME, CAT4_CODE);
#endif
#ifdef CAT5_NAME
    addCat(CAT5_NAME, CAT5_CODE);
#endif
#ifdef CAT6_NAME
    addCat(CAT6_NAME, CAT6_CODE);
#endif
}

static void mci(const char *cmd, char *ret, int n)
{
    if (!pMci) return;
    if (ret) { ret[0] = 0; pMci(cmd, ret, (UINT)n, NULL); }
    else pMci(cmd, NULL, 0, NULL);
}

static int mciInt(const char *cmd)
{
    char b[64];
    mci(cmd, b, sizeof b);
    return atoi(b);
}

static void stopTrack(void)
{
    reqGen++;
    loading = 0;
    if (isOpen) {
        mci("stop song", NULL, 0);
        mci("close song", NULL, 0);
        isOpen = 0;
    }
    if (tmpPath[0]) { remove(tmpPath); tmpPath[0] = 0; }
    playing = paused = 0;
}

static void setVolume(int v)
{
    char cmd[64];
    if (v < 0) v = 0;
    if (v > 1000) v = 1000;
    volume = v;
    if (isOpen) {
        sprintf(cmd, "setaudio song volume to %d", volume);
        mci(cmd, NULL, 0);
    }
}

static void startPlayback(void)
{
    char cmd[1200];
    sprintf(cmd, "open \"%s\" alias song", tmpPath);
    if (!pMci || pMci(cmd, NULL, 0, NULL)) {
        strcpy(playErr, "Lecture impossible (format non supporte ?)");
        remove(tmpPath); tmpPath[0] = 0;
        return;
    }
    isOpen = 1;
    setVolume(volume);
    mci("play song", NULL, 0);
    playing = 1;
    paused = 0;
}

static void playTrack(int i)
{
    DlArg *a;
    HANDLE th;
    const char *dot;
    char ext[16];

    stopTrack();
    playErr[0] = 0;
    if (i < 0 || i >= ntracks) return;
    cur = i;

    if (cur < scroll) scroll = cur;
    if (cur >= scroll + VIS) scroll = cur - VIS + 1;

    a = (DlArg *)malloc(sizeof *a);
    if (!a) return;
    dot = strrchr(tracks[i].url, '.');
    if (dot && strlen(dot) <= 5 && !strchr(dot, '/')) strcpy(ext, dot); else strcpy(ext, ".mp3");
    GetTempPathA(MAX_PATH - 40, tmpPath);
    sprintf(tmpPath + strlen(tmpPath), "sm_%lu_%d%s", (unsigned long)GetCurrentProcessId(), reqGen, ext);
    strcpy(a->path, tmpPath);
    strcpy(a->url, tracks[i].url);
    a->gen = reqGen;
    loading = 1;
    th = CreateThread(NULL, 0, dlThread, a, 0, NULL);
    if (!th) {
        free(a);
        loading = 0;
        tmpPath[0] = 0;
        strcpy(playErr, "Erreur interne (thread)");
        return;
    }
    CloseHandle(th);
}

static void togglePlay(void)
{
    if (!ntracks || loading) return;
    if (!isOpen) { playTrack(cur >= 0 ? cur : 0); return; }
    if (paused) { mci("resume song", NULL, 0); paused = 0; playing = 1; }
    else        { mci("pause song",  NULL, 0); paused = 1; }
}

static void nextTrack(void)
{
    if (!ntracks) return;
    if (shuffle && ntracks > 1) {
        int n;
        do { n = rand() % ntracks; } while (n == cur);
        playTrack(n);
    } else {
        playTrack(cur < 0 ? 0 : (cur + 1) % ntracks);
    }
}

static void prevTrack(void) { if (ntracks) playTrack(cur <= 0 ? ntracks - 1 : cur - 1); }

static void seekTo(int x)
{
    int len, pos;
    char cmd[64];
    if (!isOpen) return;
    len = mciInt("status song length");
    if (len <= 0) return;
    pos = (int)((double)(x - rSeek.left) / (rSeek.right - rSeek.left) * len);
    if (pos < 0) pos = 0;
    if (pos >= len) pos = len - 1;
    sprintf(cmd, "play song from %d", pos);
    mci(cmd, NULL, 0);
    paused = 0;
    playing = 1;
}

static void applyUpdate(void)
{
    char oldp[MAX_PATH + 8];
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    updPending = 0;
    if (!selfPath[0] || !updTmp[0]) return;
    sprintf(oldp, "%s.old", selfPath);
    DeleteFileA(oldp);
    if (!MoveFileExA(selfPath, oldp, MOVEFILE_REPLACE_EXISTING)) { remove(updTmp); return; }
    if (!MoveFileExA(updTmp, selfPath, MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED)) {
        MoveFileExA(oldp, selfPath, MOVEFILE_REPLACE_EXISTING);
        remove(updTmp);
        return;
    }
    memset(&si, 0, sizeof si);
    si.cb = sizeof si;
    memset(&pi, 0, sizeof pi);
    if (CreateProcessA(selfPath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        PostMessageA(hwnd, WM_CLOSE, 0, 0);
    }
}

static void startPlaylist(void)
{
    HANDLE th;
    stopTrack();
    ntracks = 0; tn = 0; scroll = 0; cur = -1;
    plState = 0;
    plErr[0] = 0;
    playErr[0] = 0;
    plBusy = 1;
    th = CreateThread(NULL, 0, plThread, NULL, 0, NULL);
    if (th) CloseHandle(th);
    else { plBusy = 0; strcpy(plErr, "Erreur interne (thread)"); plState = 2; }
}

static void goBack(void)
{
    stopTrack();
    screen = SCR_CATS;
    codeLen = 0; codeInput[0] = 0;
    info[0] = 0;
    drag = H_NONE;
    hot = H_NONE;
    if (updPending) applyUpdate();
}

static void enterCat(int i)
{
    if (i < 0 || i >= ncat) return;
    selCat = i;
    codeLen = 0; codeInput[0] = 0;
    info[0] = 0;
    if (catLocked[i]) strcpy(info, "Trop d'essais : relance l'application");
    screen = SCR_CODE;
}

static HFONT mkfont(int h, int bold, const char *face)
{
    return CreateFontA(-h, 0, 0, 0, bold ? FW_BOLD : FW_NORMAL, 0, 0, 0,
                       DEFAULT_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0, face);
}

static void txt(HDC d, const char *s, int x, int y, int w, int h,
                HFONT f, COLORREF c, UINT fmt)
{
    RECT r;
    r.left = x; r.top = y; r.right = x + w; r.bottom = y + h;
    SelectObject(d, f);
    SetTextColor(d, c);
    DrawTextA(d, s, -1, &r, fmt | DT_NOPREFIX);
}

static void txtShadow(HDC d, const char *s, int x, int y, int w, int h,
                      HFONT f, COLORREF c, UINT fmt)
{
    txt(d, s, x + 2, y + 2, w, h, f, RGB(0, 0, 0), fmt);
    txt(d, s, x, y, w, h, f, c, fmt);
}

static HDC      memDC = NULL;
static HBITMAP  memBmp = NULL;
static unsigned int *pixels = NULL;
static int      pixW = 0, pixH = 0;

static int ensureBackbuffer(HDC hdc, int w, int h)
{
    BITMAPINFO bi;
    if (memDC) return 1;
    memset(&bi, 0, sizeof bi);
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    memDC = CreateCompatibleDC(hdc);
    memBmp = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, (void **)&pixels, NULL, 0);
    if (!memDC || !memBmp || !pixels) return 0;
    SelectObject(memDC, memBmp);
    pixW = w; pixH = h;
    return 1;
}

static void alphaRectR(HDC d, const RECT *r, COLORREF c, int a, int rad)
{
    int x, y, x0, y0, x1, y1;
    unsigned sr = c & 255, sg = (c >> 8) & 255, sb = (c >> 16) & 255;
    (void)d;
    if (!pixels) return;
    if (a < 0) a = 0;
    if (a > 255) a = 255;
    x0 = r->left < 0 ? 0 : r->left;
    y0 = r->top < 0 ? 0 : r->top;
    x1 = r->right > pixW ? pixW : r->right;
    y1 = r->bottom > pixH ? pixH : r->bottom;
    if (x0 >= x1 || y0 >= y1) return;
    GdiFlush();
    for (y = y0; y < y1; y++) {
        unsigned int *row = pixels + (size_t)y * pixW;
        for (x = x0; x < x1; x++) {
            unsigned p, nr, ng, nb;
            if (rad > 0) {
                int cx = -1, cy = -1;
                if (x < r->left + rad)        cx = r->left + rad;
                else if (x >= r->right - rad) cx = r->right - rad - 1;
                if (y < r->top + rad)         cy = r->top + rad;
                else if (y >= r->bottom - rad) cy = r->bottom - rad - 1;
                if (cx >= 0 && cy >= 0) {
                    int dx = x - cx, dy = y - cy;
                    if (dx * dx + dy * dy > rad * rad) continue;
                }
            }
            p = row[x];
            nr = (sr * a + ((p >> 16) & 255) * (255 - a)) / 255;
            ng = (sg * a + ((p >> 8) & 255) * (255 - a)) / 255;
            nb = (sb * a + (p & 255) * (255 - a)) / 255;
            row[x] = (nr << 16) | (ng << 8) | nb;
        }
    }
}

static void alphaRect(HDC d, const RECT *r, COLORREF c, int a)
{
    alphaRectR(d, r, c, a, 0);
}

static COLORREF mixCol(COLORREF a, COLORREF b, int pct);

static void panel(HDC d, const RECT *pr, COLORREF border, int a, int bw)
{
    RECT r = *pr;
    HPEN p;
    HGDIOBJ op, ob;
    alphaRectR(d, &r, mixCol(accent, RGB(0, 0, 0), 93), a, 9);

    p = CreatePen(PS_SOLID, bw, border);
    op = SelectObject(d, p);
    ob = SelectObject(d, GetStockObject(NULL_BRUSH));
    RoundRect(d, r.left, r.top, r.right, r.bottom, 18, 18);
    SelectObject(d, op);
    SelectObject(d, ob);
    DeleteObject(p);
}

static void drawBg(HDC d, int w, int h)
{
    if (bg) {
        HDC s = CreateCompatibleDC(d);
        HGDIOBJ o = SelectObject(s, bg);
        int sx, sy, sw, sh;
        if (bgW * h > bgH * w) { sh = bgH; sw = bgH * w / h; sx = (bgW - sw) / 2; sy = 0; }
        else                   { sw = bgW; sh = bgW * h / w; sx = 0; sy = (bgH - sh) / 2; }
        SetStretchBltMode(d, HALFTONE);
        SetBrushOrgEx(d, 0, 0, NULL);
        StretchBlt(d, 0, 0, w, h, s, sx, sy, sw, sh, SRCCOPY);
        SelectObject(s, o);
        DeleteDC(s);
    } else {
        int y;
        for (y = 0; y < h; y += 4) {
            int t = y * 255 / h;
            HBRUSH b = CreateSolidBrush(RGB(60 - 52 * t / 255, 15 - 9 * t / 255, 95 - 71 * t / 255));
            RECT r; r.left = 0; r.top = y; r.right = w; r.bottom = y + 4;
            FillRect(d, &r, b);
            DeleteObject(b);
        }
    }
}

static COLORREF mixCol(COLORREF a, COLORREF b, int pct)
{
    int r = (GetRValue(a) * (100 - pct) + GetRValue(b) * pct) / 100;
    int g = (GetGValue(a) * (100 - pct) + GetGValue(b) * pct) / 100;
    int bl = (GetBValue(a) * (100 - pct) + GetBValue(b) * pct) / 100;
    return RGB(r, g, bl);
}

static void frame(HDC d, const RECT *r, COLORREF c, int w, int rad)
{
    HPEN p = CreatePen(PS_SOLID, w, c);
    HGDIOBJ op = SelectObject(d, p);
    HGDIOBJ ob = SelectObject(d, GetStockObject(NULL_BRUSH));
    RoundRect(d, r->left, r->top, r->right, r->bottom, rad, rad);
    SelectObject(d, op); SelectObject(d, ob);
    DeleteObject(p);
}

static void pill(HDC d, const RECT *r, const char *label, COLORREF col, int a, int isHot)
{
    RECT rr = *r;
    int al = isHot ? (a + 55 > 255 ? 255 : a + 55) : a;
    alphaRectR(d, &rr, col, al, (rr.bottom - rr.top) / 2);
    txtShadow(d, label, rr.left, rr.top, rr.right - rr.left, rr.bottom - rr.top,
              fMidB, RGB(255, 255, 255), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

static void card(HDC d, const RECT *pr, const char *label, COLORREF col, int isHot)
{
    RECT r = *pr;
    RECT stripe;
    panel(d, &r, isHot ? col : mixCol(col, RGB(0, 0, 0), 50), isHot ? 200 : 150, 2);
    if (isHot) alphaRectR(d, &r, col, 55, 9);
    stripe.left = r.left + 14; stripe.right = r.left + (isHot ? 24 : 19);
    stripe.top = r.top + 12;   stripe.bottom = r.bottom - 12;
    alphaRect(d, &stripe, col, 255);
    txtShadow(d, label, r.left + 30, r.top, r.right - r.left - 40, r.bottom - r.top,
              fBig, RGB(255, 255, 255), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

static void circle(HDC d, const RECT *pr, COLORREF fill, COLORREF border)
{
    RECT r = *pr;
    HBRUSH b = CreateSolidBrush(fill);
    HPEN p = CreatePen(PS_SOLID, 2, border);
    HGDIOBJ ob = SelectObject(d, b), op = SelectObject(d, p);
    Ellipse(d, r.left, r.top, r.right, r.bottom);
    SelectObject(d, ob); SelectObject(d, op);
    DeleteObject(b); DeleteObject(p);
}

static void ring(HDC d, int cx, int cy, int r, COLORREF c, int w)
{
    HPEN p = CreatePen(PS_SOLID, w, c);
    HGDIOBJ op = SelectObject(d, p);
    HGDIOBJ ob = SelectObject(d, GetStockObject(NULL_BRUSH));
    Ellipse(d, cx - r, cy - r, cx + r, cy + r);
    SelectObject(d, op); SelectObject(d, ob);
    DeleteObject(p);
}

static void icon(HDC d, int kind, const RECT *pr, COLORREF c)
{
    RECT r = *pr;
    HBRUSH b = CreateSolidBrush(c);
    HGDIOBJ ob = SelectObject(d, b);
    HGDIOBJ op = SelectObject(d, GetStockObject(NULL_PEN));
    int cx = (r.left + r.right) / 2, cy = (r.top + r.bottom) / 2;
    POINT p[3];
    switch (kind) {
    case 0:
        Rectangle(d, cx - 9, cy - 8, cx - 6, cy + 9);
        p[0].x = cx + 8; p[0].y = cy - 8; p[1].x = cx + 8; p[1].y = cy + 8; p[2].x = cx - 5; p[2].y = cy;
        Polygon(d, p, 3);
        break;
    case 1:
        p[0].x = cx - 5; p[0].y = cy - 9; p[1].x = cx - 5; p[1].y = cy + 9; p[2].x = cx + 9; p[2].y = cy;
        Polygon(d, p, 3);
        break;
    case 2:
        Rectangle(d, cx - 7, cy - 8, cx - 2, cy + 9);
        Rectangle(d, cx + 3, cy - 8, cx + 8, cy + 9);
        break;
    case 3:
        Rectangle(d, cx + 6, cy - 8, cx + 9, cy + 9);
        p[0].x = cx - 8; p[0].y = cy - 8; p[1].x = cx - 8; p[1].y = cy + 8; p[2].x = cx + 5; p[2].y = cy;
        Polygon(d, p, 3);
        break;
    case 4:
        Rectangle(d, cx - 7, cy - 7, cx + 8, cy + 8);
        break;
    }
    SelectObject(d, ob); SelectObject(d, op);
    DeleteObject(b);
}

static void fmtTime(int ms, char *out)
{
    int s = ms / 1000;
    sprintf(out, "%02d:%02d", s / 60, s % 60);
}

static void layout(void)
{
    int i;
    nbtn = 0;

    rMin.left = WIN_W - 84;   rMin.top = 4; rMin.right = WIN_W - 56;   rMin.bottom = 30;
    rClose.left = WIN_W - 50; rClose.top = 4; rClose.right = WIN_W - 22; rClose.bottom = 30;

    if (screen == SCR_CATS) {
        for (i = 0; i < ncat; i++) {
            btn[i].left = 260; btn[i].right = 700;
            btn[i].top = 190 + i * 64; btn[i].bottom = btn[i].top + 52;
        }
        nbtn = ncat;
    } else if (screen == SCR_CODE) {
        btn[0].left = 300; btn[0].top = 384; btn[0].right = 660; btn[0].bottom = 430;
        btn[1].left = 300; btn[1].top = 440; btn[1].right = 660; btn[1].bottom = 486;
        nbtn = 2;
    } else if (screen == SCR_LIST) {
        rBack.left = 760; rBack.top = 62; rBack.right = 900; rBack.bottom = 104;
        rList.left = 60;  rList.top = 120; rList.right = 560; rList.bottom = 470;
        rNow.left = 580;  rNow.top = 120; rNow.right = 900; rNow.bottom = 470;
        rPlayer.left = 60; rPlayer.top = 486; rPlayer.right = 900; rPlayer.bottom = 598;
        rSeek.left = 130; rSeek.top = 534; rSeek.right = 830; rSeek.bottom = 554;
        rPrev.left = 382; rPrev.top = 556; rPrev.right = 422; rPrev.bottom = 594;
        rPlay.left = 434; rPlay.top = 556; rPlay.right = 474; rPlay.bottom = 594;
        rNext.left = 486; rNext.top = 556; rNext.right = 526; rNext.bottom = 594;
        rStop.left = 538; rStop.top = 556; rStop.right = 578; rStop.bottom = 594;
        rShuf.left = 330; rShuf.top = 556; rShuf.right = 370; rShuf.bottom = 594;
        rRep.left  = 590; rRep.top  = 556; rRep.right  = 630; rRep.bottom  = 594;
        rVol.left = 700;  rVol.top = 566; rVol.right = 870; rVol.bottom = 584;
    }
}

static void drawCatScreen(HDC d)
{
    int i;
    RECT bar;

    txtShadow(d, "ShinraMusic", 0, 70, WIN_W, 90, fTitle, RGB(255, 255, 255), DT_CENTER | DT_SINGLELINE);
    bar.left = WIN_W / 2 - 70; bar.right = WIN_W / 2 + 70; bar.top = 139; bar.bottom = 142;
    alphaRect(d, &bar, accent, 255);

    for (i = 0; i < ncat; i++)
        card(d, &btn[i], cats[i].name, accent, hot == i);
}

static void drawLock(HDC d, int cx, int cy, COLORREF c)
{
    HPEN p = CreatePen(PS_SOLID, 4, c);
    HBRUSH b = CreateSolidBrush(c);
    HGDIOBJ op = SelectObject(d, p);
    HGDIOBJ ob = SelectObject(d, GetStockObject(NULL_BRUSH));
    RECT k;

    Ellipse(d, cx - 12, cy - 22, cx + 12, cy + 6);
    SelectObject(d, b);
    SelectObject(d, GetStockObject(NULL_PEN));
    RoundRect(d, cx - 18, cy - 4, cx + 18, cy + 22, 8, 8);
    SelectObject(d, op); SelectObject(d, ob);
    DeleteObject(p); DeleteObject(b);

    k.left = cx - 4; k.right = cx + 4; k.top = cy + 3; k.bottom = cy + 11;
    circle(d, &k, RGB(20, 20, 30), RGB(20, 20, 30));
}

static void drawCodeScreen(HDC d)
{
    RECT pn, s, u, k;
    int i, lim, sw, gap, total, x0, y0, caret, w;
    COLORREF edge;

    if (selCat < 0 || selCat >= ncat) return;

    pn.left = 240; pn.top = 100; pn.right = 720; pn.bottom = 520;
    w = pn.right - pn.left;
    panel(d, &pn, mixCol(accent, RGB(0, 0, 0), 35), 140, 2);

    drawLock(d, WIN_W / 2, 150, accent);
    txtShadow(d, cats[selCat].name, pn.left, 188, w, 44, fBig, RGB(255, 255, 255), DT_CENTER | DT_SINGLELINE);
    txt(d, "Entre ton code d'acces", pn.left, 236, w, 26, fMid, RGB(215, 215, 235), DT_CENTER | DT_SINGLELINE);

    lim = (int)strlen(cats[selCat].code);
    gap = 8;
    sw = (440 - (lim - 1) * gap) / lim;
    if (sw > 46) sw = 46;
    total = lim * sw + (lim - 1) * gap;
    x0 = (WIN_W - total) / 2;
    y0 = 280;
    caret = (int)((GetTickCount() / 500) % 2);

    for (i = 0; i < lim; i++) {
        s.left = x0 + i * (sw + gap); s.right = s.left + sw; s.top = y0; s.bottom = y0 + 56;
        alphaRectR(d, &s, accent, i < codeLen ? 80 : 30, 10);
        if (i == codeLen) edge = accent;
        else if (i < codeLen) edge = mixCol(accent, RGB(0, 0, 0), 20);
        else edge = mixCol(accent, RGB(0, 0, 0), 60);
        frame(d, &s, edge, 2, 16);
        if (i < codeLen) {
            int cx = (s.left + s.right) / 2, cy = (s.top + s.bottom) / 2;
            k.left = cx - 6; k.right = cx + 6; k.top = cy - 6; k.bottom = cy + 6;
            circle(d, &k, RGB(255, 255, 255), accent);
        } else if (i == codeLen && caret) {
            u.left = s.left + 8; u.right = s.right - 8; u.top = s.bottom - 11; u.bottom = s.bottom - 8;
            alphaRect(d, &u, accent, 255);
        }
    }

    if (info[0])
        txt(d, info, pn.left, 346, w, 26, fMid, RGB(255, 120, 120), DT_CENTER | DT_SINGLELINE);

    pill(d, &btn[0], "VALIDER", accent, 235, hot == 0);
    pill(d, &btn[1], "RETOUR", RGB(0, 0, 0), 100, hot == 1);
    frame(d, &btn[1], mixCol(accent, RGB(0, 0, 0), hot == 1 ? 0 : 55), 2, 46);

    txt(d, "Ctrl+V pour coller le code", pn.left, 494, w, 20, fSm, RGB(170, 170, 195), DT_CENTER | DT_SINGLELINE);
}

static void drawNow(HDC d)
{
    int cx = (rNow.left + rNow.right) / 2, cy = rNow.top + 100;
    int i, active = (cur >= 0 && cur < ntracks);
    int live = (isOpen && playing && !paused);
    char buf[300], nfo[96];
    RECT dr, lab;
    COLORREF edge = active ? accent : RGB(90, 90, 110);

    panel(d, &rNow, accent, 165, 2);

    dr.left = cx - 80; dr.top = cy - 80; dr.right = cx + 80; dr.bottom = cy + 80;
    circle(d, &dr, RGB(14, 14, 24), edge);
    ring(d, cx, cy, 66, RGB(42, 42, 60), 1);
    ring(d, cx, cy, 52, RGB(36, 36, 52), 1);
    lab.left = cx - 34; lab.top = cy - 34; lab.right = cx + 34; lab.bottom = cy + 34;
    circle(d, &lab, active ? accent : RGB(70, 70, 90), RGB(14, 14, 24));
    if (active) {
        char c = tracks[cur].name[0];
        if (c >= 'a' && c <= 'z') c -= 32;
        if (!((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))) c = '#';
        nfo[0] = c; nfo[1] = 0;
        txt(d, nfo, lab.left, lab.top, 68, 68, fBig, RGB(255, 255, 255), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    if (active) strcpy(buf, tracks[cur].name);
    else strcpy(buf, "Aucun titre");
    txt(d, buf, rNow.left + 14, cy + 92, rNow.right - rNow.left - 28, 58,
        fMidB, RGB(255, 255, 255), DT_CENTER | DT_WORDBREAK | DT_END_ELLIPSIS);

    if (active) sprintf(nfo, "%d / %d%s%s", cur + 1, ntracks,
                        shuffle ? "     ALEATOIRE" : "", repeat ? "     REPETE" : "");
    else nfo[0] = 0;
    txt(d, nfo, rNow.left + 10, cy + 152, rNow.right - rNow.left - 20, 22,
        fSm, RGB(200, 200, 220), DT_CENTER | DT_SINGLELINE);

    for (i = 0; i < 16; i++) {
        unsigned v = (unsigned)(GetTickCount() / 140 + i * 37) * 2654435761u;
        int h = live ? 5 + (int)((v >> 24) % 40) : 4;
        RECT b;
        b.left = cx - 141 + i * 18; b.right = b.left + 12;
        b.bottom = rNow.bottom - 12; b.top = b.bottom - h;
        alphaRect(d, &b, accent, live ? 235 : 90);
    }
}

static void drawList(HDC d)
{
    int r, len = 0, pos = 0;
    char buf[300], num[16], t1[16], t2[16];
    RECT row;

    txtShadow(d, catName, 60, 38, 640, 44, fBig, accent, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    alphaRectR(d, &rBack, RGB(255, 255, 255), hot == H_BACK ? 85 : 35, 10);
    txt(d, "< RETOUR", rBack.left, rBack.top, rBack.right - rBack.left, rBack.bottom - rBack.top,
        fMidB, RGB(255, 255, 255), DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    panel(d, &rList, accent, 165, 2);

    if (!ntracks) {
        const char *m = plState == 0 ? "Chargement de la playlist..." :
                        (plErr[0] ? plErr : "Aucun titre.");
        txt(d, m, rList.left + 20, rList.top, rList.right - rList.left - 40, rList.bottom - rList.top,
            fMid, RGB(220, 220, 235), DT_CENTER | DT_VCENTER | DT_WORDBREAK);
    } else {
        for (r = 0; r < VIS; r++) {
            int idx = scroll + r;
            if (idx >= ntracks) break;
            row.left = rList.left + 10; row.right = rList.right - 20;
            row.top = rList.top + 6 + r * ROWH; row.bottom = row.top + ROWH - 4;

            if (idx == cur) {
                RECT st;
                alphaRect(d, &row, accent, 100);
                st.left = row.left; st.right = row.left + 4; st.top = row.top; st.bottom = row.bottom;
                alphaRect(d, &st, accent, 255);
            } else if (hot == 100 + idx) alphaRect(d, &row, RGB(255, 255, 255), 45);

            sprintf(num, "%02d", idx + 1);
            txt(d, num, row.left + 12, row.top, 40, ROWH - 4, fSm,
                idx == cur ? RGB(255, 255, 255) : RGB(170, 170, 195), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            txt(d, tracks[idx].name, row.left + 56, row.top, row.right - row.left - 70, ROWH - 4, fMid,
                RGB(255, 255, 255), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            if (idx == cur && playing && !paused) {
                POINT p[3];
                HBRUSH b = CreateSolidBrush(RGB(255, 255, 255));
                HGDIOBJ ob = SelectObject(d, b), op = SelectObject(d, GetStockObject(NULL_PEN));
                p[0].x = row.right - 26; p[0].y = row.top + 7;
                p[1].x = row.right - 26; p[1].y = row.top + 23;
                p[2].x = row.right - 12; p[2].y = row.top + 15;
                Polygon(d, p, 3);
                SelectObject(d, ob); SelectObject(d, op); DeleteObject(b);
            }
        }
        if (ntracks > VIS) {
            RECT tr, th;
            int th_h = (rList.bottom - rList.top - 24) * VIS / ntracks;
            int th_y;
            if (th_h < 24) th_h = 24;
            th_y = rList.top + 12 + (rList.bottom - rList.top - 24 - th_h) * scroll / (ntracks - VIS);
            tr.left = rList.right - 12; tr.right = rList.right - 8; tr.top = rList.top + 12; tr.bottom = rList.bottom - 12;
            th.left = tr.left; th.right = tr.right; th.top = th_y; th.bottom = th_y + th_h;
            alphaRect(d, &tr, RGB(255, 255, 255), 40);
            alphaRect(d, &th, accent, 230);
        }
    }

    drawNow(d);

    panel(d, &rPlayer, accent, 190, 2);

    if (playErr[0]) strcpy(buf, playErr);
    else if (loading) strcpy(buf, "Chargement du titre...");
    else if (cur >= 0 && cur < ntracks) strcpy(buf, tracks[cur].name);
    else strcpy(buf, "Choisis un titre dans la liste");
    txt(d, buf, 90, 496, 780, 30, fMid, RGB(255, 255, 255), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

    if (isOpen) {
        len = mciInt("status song length");
        pos = mciInt("status song position");
    }
    {
        RECT bar, fill;
        int w = rSeek.right - rSeek.left;
        int fw = len > 0 ? (int)((double)pos / len * w) : 0;
        bar.left = rSeek.left; bar.right = rSeek.right; bar.top = rSeek.top + 7; bar.bottom = rSeek.top + 13;
        alphaRect(d, &bar, RGB(255, 255, 255), 70);
        fill = bar; fill.right = bar.left + fw;
        alphaRect(d, &fill, accent, 255);
        if (isOpen) {
            RECT k; k.left = bar.left + fw - 7; k.right = bar.left + fw + 7; k.top = rSeek.top + 3; k.bottom = rSeek.top + 17;
            circle(d, &k, RGB(255, 255, 255), accent);
        }
    }
    fmtTime(pos, t1); fmtTime(len, t2);
    txt(d, t1, 66, 531, 60, 26, fSm, RGB(215, 215, 235), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
    txt(d, t2, 836, 531, 60, 26, fSm, RGB(215, 215, 235), DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    circle(d, &rPrev, RGB(25, 25, 45), hot == H_PREV ? RGB(255, 255, 255) : RGB(140, 140, 170));
    icon(d, 0, &rPrev, RGB(255, 255, 255));
    circle(d, &rPlay, accent, hot == H_PLAY ? RGB(255, 255, 255) : accent);
    icon(d, (playing && !paused) ? 2 : 1, &rPlay, RGB(255, 255, 255));
    circle(d, &rNext, RGB(25, 25, 45), hot == H_NEXT ? RGB(255, 255, 255) : RGB(140, 140, 170));
    icon(d, 3, &rNext, RGB(255, 255, 255));
    circle(d, &rStop, RGB(25, 25, 45), hot == H_STOP ? RGB(255, 255, 255) : RGB(140, 140, 170));
    icon(d, 4, &rStop, RGB(255, 255, 255));

    circle(d, &rShuf, shuffle ? accent : RGB(25, 25, 45), hot == H_SHUF ? RGB(255, 255, 255) : RGB(140, 140, 170));
    txt(d, "S", rShuf.left, rShuf.top, 40, 38, fSm, RGB(255, 255, 255), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    circle(d, &rRep, repeat ? accent : RGB(25, 25, 45), hot == H_REP ? RGB(255, 255, 255) : RGB(140, 140, 170));
    txt(d, "R", rRep.left, rRep.top, 40, 38, fSm, RGB(255, 255, 255), DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    txt(d, "VOL", 650, 562, 45, 26, fSm, RGB(215, 215, 235), DT_LEFT | DT_SINGLELINE);
    {
        RECT bar, fill;
        int w = rVol.right - rVol.left;
        int fw = volume * w / 1000;
        RECT k;
        bar.left = rVol.left; bar.right = rVol.right; bar.top = rVol.top + 6; bar.bottom = rVol.top + 12;
        alphaRect(d, &bar, RGB(255, 255, 255), 70);
        fill = bar; fill.right = bar.left + fw;
        alphaRect(d, &fill, accent, 255);
        k.left = bar.left + fw - 6; k.right = bar.left + fw + 6; k.top = rVol.top + 3; k.bottom = rVol.top + 15;
        circle(d, &k, RGB(255, 255, 255), accent);
    }
}

static void drawTitleBar(HDC d, int w, int h)
{
    RECT g;
    HPEN p, op;
    HGDIOBJ ob;
    COLORREF dim;
    int y, cx, cy = TITLE_H / 2;

    (void)g; (void)y;

    txtShadow(d, "ShinraMusic", 20, 0, 260, TITLE_H, fMidB, RGB(255, 255, 255), DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    alphaRectR(d, &rMin, RGB(255, 255, 255), hot == H_MIN ? 85 : 30, 8);
    if (hot == H_CLOSE) alphaRectR(d, &rClose, RGB(232, 17, 35), 235, 8);
    else                alphaRectR(d, &rClose, RGB(255, 255, 255), 30, 8);

    p = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
    op = SelectObject(d, p);
    cx = (rMin.left + rMin.right) / 2;
    MoveToEx(d, cx - 5, cy + 3, NULL); LineTo(d, cx + 6, cy + 3);
    cx = (rClose.left + rClose.right) / 2;
    MoveToEx(d, cx - 5, cy - 5, NULL); LineTo(d, cx + 6, cy + 6);
    MoveToEx(d, cx + 5, cy - 5, NULL); LineTo(d, cx - 6, cy + 6);
    SelectObject(d, op);
    DeleteObject(p);

    dim = RGB(GetRValue(accent) * 55 / 100, GetGValue(accent) * 55 / 100, GetBValue(accent) * 55 / 100);
    p = CreatePen(PS_SOLID, 1, dim);
    op = SelectObject(d, p);
    ob = SelectObject(d, GetStockObject(NULL_BRUSH));
    RoundRect(d, 0, 0, w - 1, h - 1, 24, 24);
    SelectObject(d, op); SelectObject(d, ob);
    DeleteObject(p);
}

static void draw(HDC d, int w, int h)
{
    RECT full;
    full.left = 0; full.top = 0; full.right = w; full.bottom = h;
    SetBkMode(d, TRANSPARENT);
    drawBg(d, w, h);
    alphaRect(d, &full, RGB(0, 0, 0), 100);
    layout();
    switch (screen) {
    case SCR_CATS: drawCatScreen(d);  break;
    case SCR_CODE: drawCodeScreen(d); break;
    case SCR_LIST: drawList(d);       break;
    }
    drawTitleBar(d, w, h);
}

static COLORREF bgAccent(void)
{
    BITMAPINFO bi;
    HDC scr, sdc, ddc;
    HBITMAP db;
    unsigned int *px = NULL;
    int i, n = 64 * 40;
    double sr = 0, sg = 0, sb = 0, sw = 0, mx;
    COLORREF res = RGB(230, 230, 245);

    if (!bg || bgW <= 0 || bgH <= 0) return res;

    memset(&bi, 0, sizeof bi);
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = 64;
    bi.bmiHeader.biHeight = -40;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    scr = GetDC(NULL);
    sdc = CreateCompatibleDC(scr);
    ddc = CreateCompatibleDC(scr);
    db = CreateDIBSection(scr, &bi, DIB_RGB_COLORS, (void **)&px, NULL, 0);

    if (sdc && ddc && db && px) {
        HGDIOBJ so, dobj;
        dobj = SelectObject(ddc, db);
        so = SelectObject(sdc, bg);
        SetStretchBltMode(ddc, HALFTONE);
        SetBrushOrgEx(ddc, 0, 0, NULL);
        StretchBlt(ddc, 0, 0, 64, 40, sdc, 0, 0, bgW, bgH, SRCCOPY);
        SelectObject(sdc, so);
        SelectObject(ddc, dobj);
        GdiFlush();

        for (i = 0; i < n; i++) {
            unsigned int p = px[i];
            int r = (p >> 16) & 255, g = (p >> 8) & 255, b = p & 255;
            int hi = r > g ? (r > b ? r : b) : (g > b ? g : b);
            int lo = r < g ? (r < b ? r : b) : (g < b ? g : b);
            double w;
            if (hi < 30) continue;
            w = (double)(hi - lo);
            w = w * w + 1.0;
            sr += r * w; sg += g * w; sb += b * w; sw += w;
        }
        if (sw > 0) {
            sr /= sw; sg /= sw; sb /= sw;
            mx = sr > sg ? (sr > sb ? sr : sb) : (sg > sb ? sg : sb);
            if (mx >= 1) {
                sr = sr * 225.0 / mx * 0.92 + 20.0;
                sg = sg * 225.0 / mx * 0.92 + 20.0;
                sb = sb * 225.0 / mx * 0.92 + 20.0;
                res = RGB((int)sr, (int)sg, (int)sb);
            }
        }
    }

    if (db) DeleteObject(db);
    if (sdc) DeleteDC(sdc);
    if (ddc) DeleteDC(ddc);
    ReleaseDC(NULL, scr);
    return res;
}

static void loadBg(const char *path)
{
    HBITMAP nb;
    BITMAP bm;

    nb = (HBITMAP)LoadImageA(NULL, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (!nb) return;
    memset(&bm, 0, sizeof bm);
    GetObjectA(nb, sizeof(BITMAP), &bm);
    if (bm.bmWidth <= 0 || bm.bmHeight <= 0) { DeleteObject(nb); return; }
    if (bg) DeleteObject(bg);
    bg = nb;
    bgW = bm.bmWidth; bgH = bm.bmHeight;
    accent = bgAccent();
}

typedef BOOL (WINAPI *CLIPOPENFN)(HWND);
typedef HANDLE (WINAPI *CLIPGETFN)(UINT);
typedef BOOL (WINAPI *CLIPCLOSEFN)(void);
typedef void *(WINAPI *GLOCKFN)(HANDLE);
typedef BOOL (WINAPI *GUNLOCKFN)(HANDLE);

static void pasteCode(void)
{
    HMODULE u, k;
    CLIPOPENFN pOpen;
    CLIPGETFN pGet;
    CLIPCLOSEFN pClose;
    GLOCKFN pLock;
    GUNLOCKFN pUnlock;
    HANDLE hm;
    int lim;

    if (selCat < 0 || selCat >= ncat || catLocked[selCat]) return;
    u = LoadLibraryA("user32.dll");
    k = LoadLibraryA("kernel32.dll");
    if (!u || !k) return;
    pOpen   = (CLIPOPENFN)GetProcAddress(u, "OpenClipboard");
    pGet    = (CLIPGETFN)GetProcAddress(u, "GetClipboardData");
    pClose  = (CLIPCLOSEFN)GetProcAddress(u, "CloseClipboard");
    pLock   = (GLOCKFN)GetProcAddress(k, "GlobalLock");
    pUnlock = (GUNLOCKFN)GetProcAddress(k, "GlobalUnlock");
    if (!pOpen || !pGet || !pClose || !pLock || !pUnlock) return;

    lim = (int)strlen(cats[selCat].code);
    if (!pOpen(hwnd)) return;
    hm = pGet(CLIP_TEXT);
    if (hm) {
        const char *t = (const char *)pLock(hm);
        if (t) {
            codeLen = 0;
            codeInput[0] = 0;
            for (; *t && codeLen < lim; t++)
                if ((unsigned char)*t > 32 && (unsigned char)*t < 127)
                    codeInput[codeLen++] = *t;
            codeInput[codeLen] = 0;
            info[0] = 0;
            pUnlock(hm);
        }
    }
    pClose();
}

static int PT(const RECT *r, int x, int y)
{
    return x >= r->left && x < r->right && y >= r->top && y < r->bottom;
}

static int hitTest(int x, int y)
{
    int i;
    layout();
    if (PT(&rMin, x, y))   return H_MIN;
    if (PT(&rClose, x, y)) return H_CLOSE;
    for (i = 0; i < nbtn; i++) if (PT(&btn[i], x, y)) return i;
    if (screen == SCR_LIST) {
        if (PT(&rBack, x, y)) return H_BACK;
        if (PT(&rPrev, x, y)) return H_PREV;
        if (PT(&rPlay, x, y)) return H_PLAY;
        if (PT(&rNext, x, y)) return H_NEXT;
        if (PT(&rStop, x, y)) return H_STOP;
        if (PT(&rShuf, x, y)) return H_SHUF;
        if (PT(&rRep,  x, y)) return H_REP;
        if (PT(&rSeek, x, y)) return H_SEEK;
        if (PT(&rVol,  x, y)) return H_VOL;
        if (PT(&rList, x, y)) {
            int row = (y - rList.top - 6) / ROWH;
            int idx = scroll + row;
            if (row >= 0 && row < VIS && idx < ntracks) return 100 + idx;
        }
    }
    return H_NONE;
}

static void volFromX(int x)
{
    setVolume((x - rVol.left) * 1000 / (rVol.right - rVol.left));
}

static void submitCode(void)
{
    const char *good;
    if (selCat < 0 || selCat >= ncat) return;
    if (catLocked[selCat]) { strcpy(info, "Trop d'essais : relance l'application"); return; }
    good = cats[selCat].code;
    if (codeLen != (int)strlen(good)) { strcpy(info, "Code incomplet"); return; }
    if (plBusy) { strcpy(info, "Chargement en cours, patiente un instant..."); return; }
    if (strcmp(codeInput, good) == 0) {
        info[0] = 0;
        strcpy(catName, cats[selCat].name);
        codeLen = 0; codeInput[0] = 0;
        screen = SCR_LIST;
        startPlaylist();
    } else {
        catTries[selCat]++;
        codeLen = 0; codeInput[0] = 0;
        if (catTries[selCat] >= 5) {
            catLocked[selCat] = 1;
            strcpy(info, "Trop d'essais : relance l'application");
        } else {
            strcpy(info, "Code incorrect");
        }
    }
}

static void onClick(int id, int x)
{
    if (id == H_NONE) return;
    if (id == H_CLOSE) { PostMessageA(hwnd, WM_CLOSE, 0, 0); return; }
    if (id == H_MIN)   { ShowWindow(hwnd, SW_MINIMIZE); return; }

    if (screen == SCR_CATS) {
        if (id >= 0 && id < ncat) enterCat(id);
    } else if (screen == SCR_CODE) {
        if (id == 0) submitCode();
        else if (id == 1) goBack();
    } else if (screen == SCR_LIST) {
        if (id == H_BACK) goBack();
        else if (id >= 100 && id < 100 + MAX_TR) playTrack(id - 100);
        else if (id == H_PLAY) togglePlay();
        else if (id == H_PREV) prevTrack();
        else if (id == H_NEXT) nextTrack();
        else if (id == H_STOP) { stopTrack(); }
        else if (id == H_SHUF) shuffle = !shuffle;
        else if (id == H_REP)  repeat = !repeat;
        else if (id == H_SEEK) seekTo(x);
        else if (id == H_VOL)  { drag = H_VOL; volFromX(x); }
    }
}

static LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM wp, LPARAM lp)
{
    switch (m) {
    case WM_ERASEBKGND:
        return 1;

    case WM_NCHITTEST: {
        POINT pt;
        pt.x = (short)LOWORD(lp); pt.y = (short)HIWORD(lp);
        ScreenToClient(h, &pt);
        if (pt.y >= 0 && pt.y < TITLE_H && pt.x >= 0 && pt.x < WIN_W - 92) return HTCAPTION;
        return HTCLIENT;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(h, &ps);
        RECT rc;
        GetClientRect(h, &rc);
        if (ensureBackbuffer(hdc, rc.right, rc.bottom)) {
            draw(memDC, rc.right, rc.bottom);
            GdiFlush();
            BitBlt(hdc, 0, 0, rc.right, rc.bottom, memDC, 0, 0, SRCCOPY);
        }
        EndPaint(h, &ps);
        return 0;
    }

    case WM_MOUSEMOVE: {
        int x = (short)LOWORD(lp), y = (short)HIWORD(lp);
        int nh = hitTest(x, y);
        if (drag == H_VOL) { volFromX(x); InvalidateRect(h, NULL, FALSE); }
        if (nh != hot) { hot = nh; InvalidateRect(h, NULL, FALSE); }
        return 0;
    }

    case WM_SETCURSOR:
        if (LOWORD(lp) == HTCLIENT && hot != H_NONE) {
            SetCursor(LoadCursorA(NULL, IDC_HAND));
            return TRUE;
        }
        break;

    case WM_LBUTTONDOWN: {
        int x = (short)LOWORD(lp), y = (short)HIWORD(lp);
        SetCapture(h);
        onClick(hitTest(x, y), x);
        hot = hitTest(x, y);
        InvalidateRect(h, NULL, FALSE);
        return 0;
    }

    case WM_LBUTTONUP:
        drag = H_NONE;
        ReleaseCapture();
        return 0;

    case WM_MOUSEWHEEL:
        if (screen == SCR_LIST && ntracks > VIS) {
            int dl = (short)HIWORD(wp);
            scroll -= dl / 120;
            if (scroll < 0) scroll = 0;
            if (scroll > ntracks - VIS) scroll = ntracks - VIS;
            InvalidateRect(h, NULL, FALSE);
        }
        return 0;

    case WM_CHAR:
        if (screen == SCR_CATS) {
            char c = (char)wp;
            if (c >= '1' && c <= '9') enterCat(c - '1');
            InvalidateRect(h, NULL, FALSE);
        } else if (screen == SCR_CODE && selCat >= 0 && selCat < ncat) {
            char c = (char)wp;
            int lim = (int)strlen(cats[selCat].code);
            if (c == 27) goBack();
            else if (!catLocked[selCat]) {
                if (c == '\r') submitCode();
                else if (c == '\b') { if (codeLen > 0) codeInput[--codeLen] = 0; }
                else if (codeLen < lim && c > 32 && c < 127) {
                    codeInput[codeLen++] = c;
                    codeInput[codeLen] = 0;
                    info[0] = 0;
                }
            }
            InvalidateRect(h, NULL, FALSE);
        }
        return 0;

    case WM_KEYDOWN:
        if (screen == SCR_CODE) {
            if ((wp == 'V' && (GetKeyState(VK_CONTROL) & 0x8000)) ||
                (wp == VK_INSERT && (GetKeyState(VK_SHIFT) & 0x8000))) {
                pasteCode();
                InvalidateRect(h, NULL, FALSE);
            }
        } else if (screen == SCR_LIST) {
            if (wp == VK_SPACE) togglePlay();
            else if (wp == VK_RIGHT) nextTrack();
            else if (wp == VK_LEFT) prevTrack();
            else if (wp == VK_UP) setVolume(volume + 50);
            else if (wp == VK_DOWN) setVolume(volume - 50);
            else if (wp == 'S') shuffle = !shuffle;
            else if (wp == 'R') repeat = !repeat;
            else if (wp == VK_ESCAPE) goBack();
            InvalidateRect(h, NULL, FALSE);
        }
        return 0;

    case WM_DL_DONE:
        if ((int)lp == reqGen && loading) {
            loading = 0;
            if (wp && tmpPath[0]) startPlayback();
            else {
                strcpy(playErr, "T\xe9l\xe9" "chargement impossible (lien ou connexion ?)");
                if (tmpPath[0]) { remove(tmpPath); tmpPath[0] = 0; }
            }
        }
        InvalidateRect(h, NULL, FALSE);
        return 0;

    case WM_UPD_READY:
        if (screen == SCR_CATS && !isOpen) applyUpdate();
        else updPending = 1;
        return 0;

    case WM_BG_DONE:
        if (wp && bgTmp[0]) loadBg(bgTmp);
        if (bgTmp[0]) remove(bgTmp);
        InvalidateRect(h, NULL, FALSE);
        return 0;

    case WM_CFG_DONE:
        if (cfgN > 0) {
            int i;
            for (i = 0; i < cfgN; i++) cats[i] = cfgCats[i];
            ncat = cfgN;
            if (screen == SCR_CODE) {
                screen = SCR_CATS;
                codeLen = 0; codeInput[0] = 0;
                info[0] = 0;
            }
        }
        InvalidateRect(h, NULL, FALSE);
        return 0;

    case WM_PL_DONE:
        InvalidateRect(h, NULL, FALSE);
        return 0;

    case WM_TIMER:
        if (screen == SCR_LIST && isOpen && playing && !paused) {
            char b[32];
            mci("status song mode", b, sizeof b);
            if (!strcmp(b, "stopped")) {
                if (repeat) playTrack(cur);
                else nextTrack();
            }
        }
        if (screen == SCR_LIST || screen == SCR_CODE) InvalidateRect(h, NULL, FALSE);
        return 0;

    case WM_DESTROY:
        stopTrack();
        if (bgTmp[0]) remove(bgTmp);
        if (updTmp[0]) remove(updTmp);
        if (memBmp) DeleteObject(memBmp);
        if (memDC) DeleteDC(memDC);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(h, m, wp, lp);
}

int WINAPI WinMain(HINSTANCE hi, HINSTANCE hp, LPSTR cl, int sw)
{
    WNDCLASSA wc;
    MSG ms;
    RECT r;
    HMODULE m;
    HANDLE th;
    char exe[MAX_PATH];
    DWORD style = WS_POPUP | WS_SYSMENU | WS_MINIMIZEBOX;
    char *slash;

    (void)hp; (void)cl; (void)sw;

    SetUnhandledExceptionFilter(crashHandler);

    GetModuleFileNameA(NULL, exe, MAX_PATH);
    strcpy(selfPath, exe);
    slash = strrchr(exe, '\\');
    if (slash) { *slash = 0; SetCurrentDirectoryA(exe); }

    m = LoadLibraryA("winmm.dll");
    if (m) pMci = (MCIFN)GetProcAddress(m, "mciSendStringA");

    accent = RGB(230, 230, 245);
    loadBg("fond.bmp");

    fTitle = mkfont(54, 1, "Segoe UI");
    fBig   = mkfont(30, 1, "Segoe UI");
    fMid   = mkfont(19, 0, "Segoe UI");
    fMidB  = mkfont(21, 1, "Segoe UI");
    fSm    = mkfont(15, 0, "Segoe UI");
    fMono  = mkfont(40, 1, "Consolas");

    srand((unsigned)GetTickCount());
    initCats();

    icoBig = (HICON)LoadImageA(hi, MAKEINTRESOURCE(1), IMAGE_ICON,
                               GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), 0);
    if (!icoBig)
        icoBig = (HICON)LoadImageA(NULL, "icone.ico", IMAGE_ICON,
                                   GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_LOADFROMFILE);
    icoSm  = (HICON)LoadImageA(hi, MAKEINTRESOURCE(1), IMAGE_ICON,
                               GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
    if (!icoSm)
        icoSm = (HICON)LoadImageA(NULL, "icone.ico", IMAGE_ICON,
                                  GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_LOADFROMFILE);

    memset(&wc, 0, sizeof wc);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hi;
    wc.hIcon         = icoBig;
    wc.hCursor       = LoadCursorA(NULL, IDC_ARROW);
    wc.lpszClassName = "ShinraMusicWnd";
    if (!RegisterClassA(&wc)) { MessageBoxA(NULL, "RegisterClass a echoue", "ShinraMusic", MB_OK | MB_ICONERROR); return 1; }

    r.left = 0; r.top = 0; r.right = WIN_W; r.bottom = WIN_H;
    AdjustWindowRect(&r, style, FALSE);
    hwnd = CreateWindowA("ShinraMusicWnd", "ShinraMusic", style,
                         (GetSystemMetrics(SM_CXSCREEN) - (r.right - r.left)) / 2,
                         (GetSystemMetrics(SM_CYSCREEN) - (r.bottom - r.top)) / 2,
                         r.right - r.left, r.bottom - r.top, NULL, NULL, hi, NULL);
    if (!hwnd) {
        char b[96];
        sprintf(b, "CreateWindow a echoue, erreur %lu", (unsigned long)GetLastError());
        MessageBoxA(NULL, b, "ShinraMusic", MB_OK | MB_ICONERROR);
        return 1;
    }
    if (icoBig) SendMessageA(hwnd, WM_SETICON, ICON_BIG, (LPARAM)icoBig);
    if (icoSm)  SendMessageA(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icoSm);
    SetWindowRgn(hwnd, CreateRoundRectRgn(0, 0, r.right - r.left + 1, r.bottom - r.top + 1, 24, 24), TRUE);
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    SetTimer(hwnd, 1, 150, NULL);

    if (!strstr(GH_USER, "TONPSEUDO")) {
        th = CreateThread(NULL, 0, cfgThread, NULL, 0, NULL);
        if (th) CloseHandle(th);
        GetTempPathA(MAX_PATH - 40, bgTmp);
        sprintf(bgTmp + strlen(bgTmp), "sm_bg_%lu.bmp", (unsigned long)GetCurrentProcessId());
        th = CreateThread(NULL, 0, bgThread, NULL, 0, NULL);
        if (th) CloseHandle(th);
        GetTempPathA(MAX_PATH - 40, updTmp);
        sprintf(updTmp + strlen(updTmp), "sm_upd_%lu.exe", (unsigned long)GetCurrentProcessId());
        th = CreateThread(NULL, 0, updThread, NULL, 0, NULL);
        if (th) CloseHandle(th);
    }

    while (GetMessageA(&ms, NULL, 0, 0) > 0) {
        TranslateMessage(&ms);
        DispatchMessageA(&ms);
    }
    return 0;
}
#ifndef IDC_HAND
#define IDC_HAND MAKEINTRESOURCE(32649)
#endif
#ifndef WM_APP
#define WM_APP 0x8000
#endif
#ifndef CREATE_NO_WINDOW
#define CREATE_NO_WINDOW 0x08000000
#endif
#ifndef HANDLE_FLAG_INHERIT
#define HANDLE_FLAG_INHERIT 1
#endif
#ifndef STARTF_USESHOWWINDOW
#define STARTF_USESHOWWINDOW 0x00000001
#endif
#ifndef STARTF_USESTDHANDLES
#define STARTF_USESTDHANDLES 0x00000100
#endif
#ifndef WAIT_OBJECT_0
#define WAIT_OBJECT_0 0
#endif
#ifndef CP_ACP
#define CP_ACP 0
#endif
#ifndef CP_UTF8
#define CP_UTF8 65001
#endif

#ifndef IMAGE_ICON
#define IMAGE_ICON 1
#endif
#ifndef LR_LOADFROMFILE
#define LR_LOADFROMFILE 0x00000010
#endif
#ifndef WM_SETICON
#define WM_SETICON 0x0080
#endif
#ifndef ICON_SMALL
#define ICON_SMALL 0
#endif
#ifndef ICON_BIG
#define ICON_BIG 1
#endif
#ifndef SM_CXICON
#define SM_CXICON 11
#define SM_CYICON 12
#endif
#ifndef SM_CXSMICON
#define SM_CXSMICON 49
#define SM_CYSMICON 50
#endif

static HICON icoBig = NULL, icoSm = NULL;

#ifdef __TINYC__
int WINAPI MultiByteToWideChar(UINT, DWORD, const char *, int, unsigned short *, int);
int WINAPI WideCharToMultiByte(UINT, DWORD, const unsigned short *, int, char *, int, const char *, int *);
#endif

#define WIN_W    960
#define WIN_H    620
#define TITLE_H  34
#define CODE_MAX 12
#define MAX_CAT  6
#define MAX_TR   2048
#define ROWH     36
#define VIS      9

#define WM_DL_DONE  (WM_APP + 1)
#define WM_PL_DONE  (WM_APP + 2)
#define WM_CFG_DONE (WM_APP + 3)
#define WM_BG_DONE  (WM_APP + 4)
#define CLIP_TEXT   1
#define WM_UPD_READY (WM_APP + 5)

#ifndef APP_BUILD
#define APP_BUILD 0
#endif
#ifndef MOVEFILE_REPLACE_EXISTING
#define MOVEFILE_REPLACE_EXISTING 0x00000001
#endif
#ifndef MOVEFILE_COPY_ALLOWED
#define MOVEFILE_COPY_ALLOWED 0x00000002
#endif

#ifndef VK_INSERT
#define VK_INSERT 0x2D
#endif

typedef DWORD (WINAPI *MCIFN)(const char *, char *, UINT, void *);

static MCIFN pMci = NULL;

enum { SCR_CATS, SCR_CODE, SCR_LIST };
enum { H_NONE = -1, H_PREV = 5000, H_PLAY, H_NEXT, H_STOP, H_SEEK, H_VOL,
       H_SHUF, H_REP, H_MIN, H_CLOSE, H_BACK };

typedef struct { char name[160]; char url[512]; } Track;
typedef struct { char url[512]; char path[MAX_PATH]; int gen; } DlArg;
typedef struct { char name[48]; char code[CODE_MAX + 1]; } Cat;

static HWND    hwnd;
static int     screen = SCR_CATS;
static int     hot = H_NONE, drag = H_NONE;
static HBITMAP bg = NULL;
static int     bgW = 0, bgH = 0;
static HFONT   fTitle, fBig, fMid, fMidB, fSm, fMono;
static COLORREF accent;
static char    bgTmp[MAX_PATH] = "";
static char    selfPath[MAX_PATH] = "";
static char    updTmp[MAX_PATH] = "";
static int     updPending = 0;

static char    codeInput[CODE_MAX + 2];
static int     codeLen = 0;
static char    info[160] = "";

static Cat     cats[MAX_CAT];
static Cat     cfgCats[MAX_CAT];
static int     ncat = 0, cfgN = 0, selCat = 0;
static int     catTries[MAX_CAT], catLocked[MAX_CAT];
static char    catName[48] = "";
static volatile int plBusy = 0;
static char    cfgBuf[262144];

static RECT    btn[MAX_CAT + 2];
static int     nbtn = 0;
static RECT    rList, rPlayer, rSeek, rVol, rPrev, rPlay, rNext, rStop, rShuf, rRep;
static RECT    rNow, rMin, rClose, rBack;

static Track   tracks[MAX_TR];
static int     ntracks = 0, scroll = 0, cur = -1;
static int     tn = 0;
static volatile int plState = 0;
static char    plErr[160] = "";
static char    netBuf[1000000];

static int     isOpen = 0, playing = 0, paused = 0, volume = 700;
static int     shuffle = 0, repeat = 0;
static volatile int loading = 0;
static volatile int reqGen = 0;
static char    playErr[96] = "";
static char    tmpPath[MAX_PATH] = "";

static LONG WINAPI crashHandler(EXCEPTION_POINTERS *ep)
{
    char b[160];
    sprintf(b, "PLANTAGE code=%08lX adresse=%p",
            (unsigned long)ep->ExceptionRecord->ExceptionCode,
            ep->ExceptionRecord->ExceptionAddress);
    MessageBoxA(NULL, b, "ShinraMusic - plantage", MB_OK | MB_ICONERROR);
    return EXCEPTION_EXECUTE_HANDLER;
}

static int runCapture(const char *cmdline, char *out, int outsz)
{
    SECURITY_ATTRIBUTES sa;
    HANDLE rd = NULL, wr = NULL;
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    char cmd[3000], scratch[256];
    DWORD got = 0, code = 1;
    int n = 0;

    sa.nLength = sizeof sa;
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    if (!CreatePipe(&rd, &wr, &sa, 0)) return -1;
    SetHandleInformation(rd, HANDLE_FLAG_INHERIT, 0);

    memset(&si, 0, sizeof si);
    si.cb = sizeof si;
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = wr;
    si.hStdError = wr;
    si.hStdInput = NULL;

    strncpy(cmd, cmdline, sizeof cmd - 1);
    cmd[sizeof cmd - 1] = 0;
    memset(&pi, 0, sizeof pi);
    if (!CreateProcessA(NULL, cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        CloseHandle(rd); CloseHandle(wr);
        return -1;
    }
    CloseHandle(wr);

    if (out && outsz > 1) {
        while (n < outsz - 1 && ReadFile(rd, out + n, (DWORD)(outsz - 1 - n), &got, NULL) && got > 0)
            n += (int)got;
        out[n] = 0;
    } else {
        while (ReadFile(rd, scratch, sizeof scratch, &got, NULL) && got > 0) { }
    }
    CloseHandle(rd);

    if (WaitForSingleObject(pi.hProcess, 8000) != WAIT_OBJECT_0) TerminateProcess(pi.hProcess, 1);
    GetExitCodeProcess(pi.hProcess, &code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return (int)code;
}

static void utf8ToAnsi(const char *s, char *out, int outsz)
{
    char src[300];
    unsigned short w[320];
    strncpy(src, s, sizeof src - 1);
    src[sizeof src - 1] = 0;
    if (MultiByteToWideChar(CP_UTF8, 0, src, -1, w, 320) > 0 &&
        WideCharToMultiByte(CP_ACP, 0, w, -1, out, outsz, NULL, NULL) > 0)
        return;
    strncpy(out, src, outsz - 1);
    out[outsz - 1] = 0;
}

static int hexv(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static void urlEnc(const char *s, char *out, int outsz)
{
    int n = 0;
    for (; *s && n < outsz - 4; s++) {
        unsigned char c = (unsigned char)*s;
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
            c == '-' || c == '_' || c == '.' || c == '~' || c == '/')
            out[n++] = (char)c;
        else
            n += sprintf(out + n, "%%%02X", c);
    }
    out[n] = 0;
}

static void jsonStrU8(const char *s, char *out, int outsz)
{
    int n = 0;
    while (*s && *s != '"' && n < outsz - 4) {
        if (*s == '\\') {
            s++;
            if (*s == 'u') {
                unsigned cp = 0;
                int k, bad = 0;
                for (k = 1; k <= 4; k++) {
                    int v = hexv(s[k]);
                    if (v < 0) { bad = 1; break; }
                    cp = (cp << 4) | (unsigned)v;
                }
                if (bad) { s++; continue; }
                s += 5;
                if (cp >= 0xD800 && cp <= 0xDFFF) continue;
                if (cp < 0x80) out[n++] = (char)cp;
                else if (cp < 0x800) {
                    out[n++] = (char)(0xC0 | (cp >> 6));
                    out[n++] = (char)(0x80 | (cp & 63));
                } else {
                    out[n++] = (char)(0xE0 | (cp >> 12));
                    out[n++] = (char)(0x80 | ((cp >> 6) & 63));
                    out[n++] = (char)(0x80 | (cp & 63));
                }
                continue;
            }
            if (*s == 0) break;
            out[n++] = *s++;
        } else {
            out[n++] = *s++;
        }
    }
    out[n] = 0;
}

static void compactJson(char *s)
{
    char *w = s;
    int inStr = 0;
    for (; *s; s++) {
        if (inStr) {
            *w++ = *s;
            if (*s == '\\' && s[1]) *w++ = *++s;
            else if (*s == '"') inStr = 0;
        } else if (*s == '"') {
            inStr = 1;
            *w++ = *s;
        } else if (*s != ' ' && *s != '\n' && *s != '\r' && *s != '\t') {
            *w++ = *s;
        }
    }
    *w = 0;
}

static void parseGithub(char *buf)
{
    char *pos = buf;

    while (tn < MAX_TR && (pos = strstr(pos, "\"name\":\"")) != NULL) {
        char *nm = pos + 8, *u, *nx, *dot;
        char nameU8[300], url[512];
        Track *t;

        pos = nm;
        u = strstr(nm, "\"download_url\":\"");
        if (!u) break;
        nx = strstr(nm, "\"name\":\"");
        if (nx && nx < u) continue;
        jsonStrU8(nm, nameU8, sizeof nameU8);
        jsonStrU8(u + 16, url, sizeof url);
        dot = strrchr(nameU8, '.');
        if (!dot || !(!lstrcmpiA(dot, ".mp3") || !lstrcmpiA(dot, ".wav") ||
                      !lstrcmpiA(dot, ".m4a") || !lstrcmpiA(dot, ".wma")))
            continue;
        *dot = 0;
        if (strncmp(url, "http", 4) || strchr(url, '"') || strchr(url, ' ')) continue;
        t = &tracks[tn];
        utf8ToAnsi(nameU8, t->name, sizeof t->name);
        if (!t->name[0]) sprintf(t->name, "Titre %d", tn + 1);
        strcpy(t->url, url);
        tn++;
    }
}

static int cfgGet(const char *buf, int n, const char *field, char *out, int outsz)
{
    char key[48];
    const char *p, *e;
    int i, len;

    sprintf(key, "#define CAT%d_%s \"", n, field);
    p = strstr(buf, key);
    if (!p) return 0;
    p += strlen(key);
    e = strchr(p, '"');
    if (!e) return 0;
    len = (int)(e - p);
    if (len <= 0 || len >= outsz) return 0;
    for (i = 0; i < len; i++) if ((unsigned char)p[i] < 32) return 0;
    memcpy(out, p, len);
    out[len] = 0;
    return 1;
}

static DWORD WINAPI cfgThread(LPVOID unused)
{
    char cmd[500];
    int n;
    (void)unused;

    cfgN = 0;
    sprintf(cmd, "curl.exe -s -L -m 15 \"https://raw.githubusercontent.com/%s/%s/%s/%s\"",
            GH_USER, GH_REPO, GH_BRANCH, GH_SRC);
    if (runCapture(cmd, cfgBuf, sizeof cfgBuf) == 0 && cfgBuf[0]) {
        for (n = 1; n <= MAX_CAT; n++) {
            Cat *c = &cfgCats[cfgN];
            if (!cfgGet(cfgBuf, n, "NAME", c->name, sizeof c->name)) break;
            if (!cfgGet(cfgBuf, n, "CODE", c->code, sizeof c->code)) break;
            cfgN++;
        }
    }
    PostMessageA(hwnd, WM_CFG_DONE, 0, 0);
    return 0;
}

static DWORD WINAPI bgThread(LPVOID unused)
{
    char cmd[900];
    int rc;
    (void)unused;

    sprintf(cmd, "curl.exe -s -L -m 60 -o \"%s\" \"https://raw.githubusercontent.com/%s/%s/%s/%s\"",
            bgTmp, GH_USER, GH_REPO, GH_BRANCH, GH_BG);
    rc = runCapture(cmd, NULL, 0);
    PostMessageA(hwnd, WM_BG_DONE, (WPARAM)(rc == 0), 0);
    return 0;
}

static DWORD WINAPI updThread(LPVOID unused)
{
    char cmd[900], out[64], oldp[MAX_PATH + 8];
    int remote;
    FILE *f;
    long sz = 0;
    unsigned char hdr[2] = { 0, 0 };
    (void)unused;

    sprintf(oldp, "%s.old", selfPath);
    DeleteFileA(oldp);

    sprintf(cmd, "curl.exe -s -L -m 15 \"https://github.com/%s/%s/releases/latest/download/version.txt\"",
            GH_USER, GH_REPO);
    out[0] = 0;
    if (runCapture(cmd, out, sizeof out) != 0 || !out[0]) return 0;
    remote = atoi(out);
    if (remote <= APP_BUILD) return 0;

    sprintf(cmd, "curl.exe -s -L -m 120 -o \"%s\" \"https://github.com/%s/%s/releases/latest/download/ShinraMusic.exe\"",
            updTmp, GH_USER, GH_REPO);
    if (runCapture(cmd, NULL, 0) != 0) { remove(updTmp); return 0; }

    f = fopen(updTmp, "rb");
    if (f) {
        if (fread(hdr, 1, 2, f) != 2) hdr[0] = 0;
        fseek(f, 0, SEEK_END);
        sz = ftell(f);
        fclose(f);
    }
    if (sz < 100000 || hdr[0] != 'M' || hdr[1] != 'Z') { remove(updTmp); return 0; }

    PostMessageA(hwnd, WM_UPD_READY, 0, 0);
    return 0;
}

static DWORD WINAPI plThread(LPVOID unused)
{
    char cmd[1500], raw[200], enc[600];
    (void)unused;

    tn = 0;
    plErr[0] = 0;
    if (strstr(GH_USER, "TONPSEUDO")) {
        strcpy(plErr, "Renseigne GH_USER et GH_REPO en haut de ShinraMusic.c");
        goto done;
    }
    if (GH_DIR[0]) sprintf(raw, "%s/%s", GH_DIR, catName);
    else           strcpy(raw, catName);
    urlEnc(raw, enc, sizeof enc);
    sprintf(cmd, "curl.exe -s -L -m 25 -H \"User-Agent: ShinraMusic\" "
                 "-H \"Accept: application/vnd.github+json\" "
                 "\"https://api.github.com/repos/%s/%s/contents/%s\"",
            GH_USER, GH_REPO, enc);
    if (runCapture(cmd, netBuf, sizeof netBuf) != 0 || !netBuf[0]) {
        strcpy(plErr, "GitHub injoignable (internet ?)");
        goto done;
    }
    compactJson(netBuf);
    parseGithub(netBuf);
    if (tn == 0) {
        if (strstr(netBuf, "rate limit"))
            strcpy(plErr, "Limite GitHub atteinte (r\xe9" "essaie dans un moment)");
        else if (strstr(netBuf, "\"message\""))
            strcpy(plErr, "D\xe9p\xf4t ou dossier introuvable (d\xe9p\xf4t public ? noms exacts ?)");
        else
            strcpy(plErr, "Aucune musique (mp3, wav, m4a, wma) dans ce dossier");
    }

done:
    ntracks = tn;
    plState = tn > 0 ? 1 : 2;
    plBusy = 0;
    PostMessageA(hwnd, WM_PL_DONE, (WPARAM)tn, 0);
    return 0;
}

static DWORD WINAPI dlThread(LPVOID p)
{
    DlArg *a = (DlArg *)p;
    char cmd[1400];
    int rc;

    sprintf(cmd, "curl.exe -s -L -m 180 -o \"%s\" \"%s\"", a->path, a->url);
    rc = runCapture(cmd, NULL, 0);
    if (a->gen != reqGen) {
        remove(a->path);
        free(a);
        return 0;
    }
    PostMessageA(hwnd, WM_DL_DONE, (WPARAM)(rc == 0), (LPARAM)a->gen);
    free(a);
    return 0;
}

static void addCat(const char *name, const char *code)
{
    if (ncat >= MAX_CAT) return;
    strncpy(cats[ncat].name, name, sizeof cats[ncat].name - 1);
    cats[ncat].name[sizeof cats[ncat].name - 1] = 0;
    strncpy(cats[ncat].code, code, CODE_MAX);
    cats[ncat].code[CODE_MAX] = 0;
    ncat++;
}

static void initCats(void)
{
    ncat = 0;
#ifdef CAT1_NAME
    addCat(CAT1_NAME, CAT1_CODE);
#endif
#ifdef CAT2_NAME
    addCat(CAT2_NAME, CAT2_CODE);
#endif
#ifdef CAT3_NAME
    addCat(CAT3_NAME, CAT3_CODE);
#endif
#ifdef CAT4_NAME
    addCat(CAT4_NAME, CAT4_CODE);
#endif
#ifdef CAT5_NAME
    addCat(CAT5_NAME, CAT5_CODE);
#endif
#ifdef CAT6_NAME
    addCat(CAT6_NAME, CAT6_CODE);
#endif
}

static void mci(const char *cmd, char *ret, int n)
{
    if (!pMci) return;
    if (ret) { ret[0] = 0; pMci(cmd, ret, (UINT)n, NULL); }
    else pMci(cmd, NULL, 0, NULL);
}

static int mciInt(const char *cmd)
{
    char b[64];
    mci(cmd, b, sizeof b);
    return atoi(b);
}

static void stopTrack(void)
{
    reqGen++;
    loading = 0;
    if (isOpen) {
        mci("stop song", NULL, 0);
        mci("close song", NULL, 0);
        isOpen = 0;
    }
    if (tmpPath[0]) { remove(tmpPath); tmpPath[0] = 0; }
    playing = paused = 0;
}

static void setVolume(int v)
{
    char cmd[64];
    if (v < 0) v = 0;
    if (v > 1000) v = 1000;
    volume = v;
    if (isOpen) {
        sprintf(cmd, "setaudio song volume to %d", volume);
        mci(cmd, NULL, 0);
    }
}

static void startPlayback(void)
{
    char cmd[1200];
    sprintf(cmd, "open \"%s\" alias song", tmpPath);
    if (!pMci || pMci(cmd, NULL, 0, NULL)) {
        strcpy(playErr, "Lecture impossible (format non supporte ?)");
        remove(tmpPath); tmpPath[0] = 0;
        return;
    }
    isOpen = 1;
    setVolume(volume);
    mci("play song", NULL, 0);
    playing = 1;
    paused = 0;
}

static void playTrack(int i)
{
    DlArg *a;
    HANDLE th;
    const char *dot;
    char ext[16];

    stopTrack();
    playErr[0] = 0;
    if (i < 0 || i >= ntracks) return;
    cur = i;

    if (cur < scroll) scroll = cur;
    if (cur >= scroll + VIS) scroll = cur - VIS + 1;

    a = (DlArg *)malloc(sizeof *a);
    if (!a) return;
    dot = strrchr(tracks[i].url, '.');
    if (dot && strlen(dot) <= 5 && !strchr(dot, '/')) strcpy(ext, dot); else strcpy(ext, ".mp3");
    GetTempPathA(MAX_PATH - 40, tmpPath);
    sprintf(tmpPath + strlen(tmpPath), "sm_%lu_%d%s", (unsigned long)GetCurrentProcessId(), reqGen, ext);
    strcpy(a->path, tmpPath);
    strcpy(a->url, tracks[i].url);
    a->gen = reqGen;
    loading = 1;
    th = CreateThread(NULL, 0, dlThread, a, 0, NULL);
    if (!th) {
        free(a);
        loading = 0;
        tmpPath[0] = 0;
        strcpy(playErr, "Erreur interne (thread)");
        return;
    }
    CloseHandle(th);
}

static void togglePlay(void)
{
    if (!ntracks || loading) return;
    if (!isOpen) { playTrack(cur >= 0 ? cur : 0); return; }
    if (paused) { mci("resume song", NULL, 0); paused = 0; playing = 1; }
    else        { mci("pause song",  NULL, 0); paused = 1; }
}

static void nextTrack(void)
{
    if (!ntracks) return;
    if (shuffle && ntracks > 1) {
        int n;
        do { n = rand() % ntracks; } while (n == cur);
        playTrack(n);
    } else {
        playTrack(cur < 0 ? 0 : (cur + 1) % ntracks);
    }
}

static void prevTrack(void) { if (ntracks) playTrack(cur <= 0 ? ntracks - 1 : cur - 1); }

static void seekTo(int x)
{
    int len, pos;
    char cmd[64];
    if (!isOpen) return;
    len = mciInt("status song length");
    if (len <= 0) return;
    pos = (int)((double)(x - rSeek.left) / (rSeek.right - rSeek.left) * len);
    if (pos < 0) pos = 0;
    if (pos >= len) pos = len - 1;
    sprintf(cmd, "play song from %d", pos);
    mci(cmd, NULL, 0);
    paused = 0;
    playing = 1;
}

static void applyUpdate(void)
{
    char oldp[MAX_PATH + 8];
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    updPending = 0;
    if (!selfPath[0] || !updTmp[0]) return;
    sprintf(oldp, "%s.old", selfPath);
    DeleteFileA(oldp);
    if (!MoveFileExA(selfPath, oldp, MOVEFILE_REPLACE_EXISTING)) { remove(updTmp); return; }
    if (!MoveFileExA(updTmp, selfPath, MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED)) {
        MoveFileExA(oldp, selfPath, MOVEFILE_REPLACE_EXISTING);
        remove(updTmp);
        return;
    }
    memset(&si, 0, sizeof si);
    si.cb = sizeof si;
    memset(&pi, 0, sizeof pi);
    if (CreateProcessA(selfPath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        PostMessageA(hwnd, WM_CLOSE, 0, 0);
    }
}

static void startPlaylist(void)
{
    HANDLE th;
    stopTrack();
    ntracks = 0; tn = 0; scroll = 0; cur = -1;
    plState = 0;
    plErr[0] = 0;
    playErr[0] = 0;
    plBusy = 1;
    th = CreateThread(NULL, 0, plThread, NULL, 0, NULL);
    if (th) CloseHandle(th);
    else { plBusy = 0; strcpy(plErr, "Erreur interne (thread)"); plState = 2; }
}

static void goBack(void)
{
    stopTrack();
    screen = SCR_CATS;
    codeLen = 0; codeInput[0] = 0;
    info[0] = 0;
    drag = H_NONE;
    hot = H_NONE;
    if (updPending) applyUpdate();
}

static void enterCat(int i)
{
    if (i < 0 || i >= ncat) return;
    selCat = i;
    codeLen = 0; codeInput[0] = 0;
    info[0] = 0;
    if (catLocked[i]) strcpy(info, "Trop d'essais : relance l'application");
    screen = SCR_CODE;
}

static HFONT mkfont(int h, int bold, const char *face)
{
    return CreateFontA(-h, 0, 0, 0, bold ? FW_BOLD : FW_NORMAL, 0, 0, 0,
                       DEFAULT_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0, face);
}

static void txt(HDC d, const char *s, int x, int y, int w, int h,
                HFONT f, COLORREF c, UINT fmt)
{
    RECT r;
    r.left = x; r.top = y; r.right = x + w; r.bottom = y + h;
    SelectObject(d, f);
    SetTextColor(d, c);
    DrawTextA(d, s, -1, &r, fmt | DT_NOPREFIX);
}

static void txtShadow(HDC d, const char *s, int x, int y, int w, int h,
                      HFONT f, COLORREF c, UINT fmt)
{
    txt(d, s, x + 2, y + 2, w, h, f, RGB(0, 0, 0), fmt);
    txt(d, s, x, y, w, h, f, c, fmt);
}

static HDC      memDC = NULL;
static HBITMAP  memBmp = NULL;
static unsigned int *pixels = NULL;
static int      pixW = 0, pixH = 0;

static int ensureBackbuffer(HDC hdc, int w, int h)
{
    BITMAPINFO bi;
    if (memDC) return 1;
    memset(&bi, 0, sizeof bi);
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    memDC = CreateCompatibleDC(hdc);
    memBmp = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, (void **)&pixels, NULL, 0);
    if (!memDC || !memBmp || !pixels) return 0;
    SelectObject(memDC, memBmp);
    pixW = w; pixH = h;
    return 1;
}

static void alphaRectR(HDC d, const RECT *r, COLORREF c, int a, int rad)
{
    int x, y, x0, y0, x1, y1;
    unsigned sr = c & 255, sg = (c >> 8) & 255, sb = (c >> 16) & 255;
    (void)d;
    if (!pixels) return;
    if (a < 0) a = 0;
    if (a > 255) a = 255;
    x0 = r->left < 0 ? 0 : r->left;
    y0 = r->top < 0 ? 0 : r->top;
    x1 = r->right > pixW ? pixW : r->right;
    y1 = r->bottom > pixH ? pixH : r->bottom;
    if (x0 >= x1 || y0 >= y1) return;
    GdiFlush();
    for (y = y0; y < y1; y++) {
        unsigned int *row = pixels + (size_t)y * pixW;
        for (x = x0; x < x1; x++) {
            unsigned p, nr, ng, nb;
            if (rad > 0) {
                int cx = -1, cy = -1;
                if (x < r->left + rad)        cx = r->left + rad;
                else if (x >= r->right - rad) cx = r->right - rad - 1;
                if (y < r->top + rad)         cy = r->top + rad;
                else if (y >= r->bottom - rad) cy = r->bottom - rad - 1;
                if (cx >= 0 && cy >= 0) {
                    int dx = x - cx, dy = y - cy;
                    if (dx * dx + dy * dy > rad * rad) continue;
                }
            }
            p = row[x];
            nr = (sr * a + ((p >> 16) & 255) * (255 - a)) / 255;
            ng = (sg * a + ((p >> 8) & 255) * (255 - a)) / 255;
            nb = (sb * a + (p & 255) * (255 - a)) / 255;
            row[x] = (nr << 16) | (ng << 8) | nb;
        }
    }
}

static void alphaRect(HDC d, const RECT *r, COLORREF c, int a)
{
    alphaRectR(d, r, c, a, 0);
}

static COLORREF mixCol(COLORREF a, COLORREF b, int pct);

static void panel(HDC d, const RECT *pr, COLORREF border, int a, int bw)
{
    RECT r = *pr;
    HPEN p;
    HGDIOBJ op, ob;
    alphaRectR(d, &r, mixCol(accent, RGB(0, 0, 0), 93), a, 9);

    p = CreatePen(PS_SOLID, bw, border);
    op = SelectObject(d, p);
    ob = SelectObject(d, GetStockObject(NULL_BRUSH));
    RoundRect(d, r.left, r.top, r.right, r.bottom, 18, 18);
    SelectObject(d, op);
    SelectObject(d, ob);
    DeleteObject(p);
}

static void drawBg(HDC d, int w, int h)
{
    if (bg) {
        HDC s = CreateCompatibleDC(d);
        HGDIOBJ o = SelectObject(s, bg);
        int sx, sy, sw, sh;
        if (bgW * h > bgH * w) { sh = bgH; sw = bgH * w / h; sx = (bgW - sw) / 2; sy = 0; }
        else                   { sw = bgW; sh = bgW * h / w; sx = 0; sy = (bgH - sh) / 2; }
        SetStretchBltMode(d, HALFTONE);
        SetBrushOrgEx(d, 0, 0, NULL);
        StretchBlt(d, 0, 0, w, h, s, sx, sy, sw, sh, SRCCOPY);
        SelectObject(s, o);
        DeleteDC(s);
    } else {
        int y;
        for (y = 0; y < h; y += 4) {
            int t = y * 255 / h;
            HBRUSH b = CreateSolidBrush(RGB(60 - 52 * t / 255, 15 - 9 * t / 255, 95 - 71 * t / 255));
            RECT r; r.left = 0; r.top = y; r.right = w; r.bottom = y + 4;
            FillRect(d, &r, b);
            DeleteObject(b);
        }
    }
}

static COLORREF mixCol(COLORREF a, COLORREF b, int pct)
{
    int r = (GetRValue(a) * (100 - pct) + GetRValue(b) * pct) / 100;
    int g = (GetGValue(a) * (100 - pct) + GetGValue(b) * pct) / 100;
    int bl = (GetBValue(a) * (100 - pct) + GetBValue(b) * pct) / 100;
    return RGB(r, g, bl);
}

static void frame(HDC d, const RECT *r, COLORREF c, int w, int rad)
{
    HPEN p = CreatePen(PS_SOLID, w, c);
    HGDIOBJ op = SelectObject(d, p);
    HGDIOBJ ob = SelectObject(d, GetStockObject(NULL_BRUSH));
    RoundRect(d, r->left, r->top, r->right, r->bottom, rad, rad);
    SelectObject(d, op); SelectObject(d, ob);
    DeleteObject(p);
}

static void pill(HDC d, const RECT *r, const char *label, COLORREF col, int a, int isHot)
{
    RECT rr = *r;
    int al = isHot ? (a + 55 > 255 ? 255 : a + 55) : a;
    alphaRectR(d, &rr, col, al, (rr.bottom - rr.top) / 2);
    txtShadow(d, label, rr.left, rr.top, rr.right - rr.left, rr.bottom - rr.top,
              fMidB, RGB(255, 255, 255), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

static void card(HDC d, const RECT *pr, const char *label, COLORREF col, int isHot)
{
    RECT r = *pr;
    RECT stripe;
    panel(d, &r, isHot ? col : mixCol(col, RGB(0, 0, 0), 50), isHot ? 200 : 150, 2);
    if (isHot) alphaRectR(d, &r, col, 55, 9);
    stripe.left = r.left + 14; stripe.right = r.left + (isHot ? 24 : 19);
    stripe.top = r.top + 12;   stripe.bottom = r.bottom - 12;
    alphaRect(d, &stripe, col, 255);
    txtShadow(d, label, r.left + 30, r.top, r.right - r.left - 40, r.bottom - r.top,
              fBig, RGB(255, 255, 255), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

static void circle(HDC d, const RECT *pr, COLORREF fill, COLORREF border)
{
    RECT r = *pr;
    HBRUSH b = CreateSolidBrush(fill);
    HPEN p = CreatePen(PS_SOLID, 2, border);
    HGDIOBJ ob = SelectObject(d, b), op = SelectObject(d, p);
    Ellipse(d, r.left, r.top, r.right, r.bottom);
    SelectObject(d, ob); SelectObject(d, op);
    DeleteObject(b); DeleteObject(p);
}

static void ring(HDC d, int cx, int cy, int r, COLORREF c, int w)
{
    HPEN p = CreatePen(PS_SOLID, w, c);
    HGDIOBJ op = SelectObject(d, p);
    HGDIOBJ ob = SelectObject(d, GetStockObject(NULL_BRUSH));
    Ellipse(d, cx - r, cy - r, cx + r, cy + r);
    SelectObject(d, op); SelectObject(d, ob);
    DeleteObject(p);
}

static void icon(HDC d, int kind, const RECT *pr, COLORREF c)
{
    RECT r = *pr;
    HBRUSH b = CreateSolidBrush(c);
    HGDIOBJ ob = SelectObject(d, b);
    HGDIOBJ op = SelectObject(d, GetStockObject(NULL_PEN));
    int cx = (r.left + r.right) / 2, cy = (r.top + r.bottom) / 2;
    POINT p[3];
    switch (kind) {
    case 0:
        Rectangle(d, cx - 9, cy - 8, cx - 6, cy + 9);
        p[0].x = cx + 8; p[0].y = cy - 8; p[1].x = cx + 8; p[1].y = cy + 8; p[2].x = cx - 5; p[2].y = cy;
        Polygon(d, p, 3);
        break;
    case 1:
        p[0].x = cx - 5; p[0].y = cy - 9; p[1].x = cx - 5; p[1].y = cy + 9; p[2].x = cx + 9; p[2].y = cy;
        Polygon(d, p, 3);
        break;
    case 2:
        Rectangle(d, cx - 7, cy - 8, cx - 2, cy + 9);
        Rectangle(d, cx + 3, cy - 8, cx + 8, cy + 9);
        break;
    case 3:
        Rectangle(d, cx + 6, cy - 8, cx + 9, cy + 9);
        p[0].x = cx - 8; p[0].y = cy - 8; p[1].x = cx - 8; p[1].y = cy + 8; p[2].x = cx + 5; p[2].y = cy;
        Polygon(d, p, 3);
        break;
    case 4:
        Rectangle(d, cx - 7, cy - 7, cx + 8, cy + 8);
        break;
    }
    SelectObject(d, ob); SelectObject(d, op);
    DeleteObject(b);
}

static void fmtTime(int ms, char *out)
{
    int s = ms / 1000;
    sprintf(out, "%02d:%02d", s / 60, s % 60);
}

static void layout(void)
{
    int i;
    nbtn = 0;

    rMin.left = WIN_W - 84;   rMin.top = 4; rMin.right = WIN_W - 56;   rMin.bottom = 30;
    rClose.left = WIN_W - 50; rClose.top = 4; rClose.right = WIN_W - 22; rClose.bottom = 30;

    if (screen == SCR_CATS) {
        for (i = 0; i < ncat; i++) {
            btn[i].left = 260; btn[i].right = 700;
            btn[i].top = 190 + i * 64; btn[i].bottom = btn[i].top + 52;
        }
        nbtn = ncat;
    } else if (screen == SCR_CODE) {
        btn[0].left = 300; btn[0].top = 384; btn[0].right = 660; btn[0].bottom = 430;
        btn[1].left = 300; btn[1].top = 440; btn[1].right = 660; btn[1].bottom = 486;
        nbtn = 2;
    } else if (screen == SCR_LIST) {
        rBack.left = 760; rBack.top = 62; rBack.right = 900; rBack.bottom = 104;
        rList.left = 60;  rList.top = 120; rList.right = 560; rList.bottom = 470;
        rNow.left = 580;  rNow.top = 120; rNow.right = 900; rNow.bottom = 470;
        rPlayer.left = 60; rPlayer.top = 486; rPlayer.right = 900; rPlayer.bottom = 598;
        rSeek.left = 130; rSeek.top = 534; rSeek.right = 830; rSeek.bottom = 554;
        rPrev.left = 382; rPrev.top = 556; rPrev.right = 422; rPrev.bottom = 594;
        rPlay.left = 434; rPlay.top = 556; rPlay.right = 474; rPlay.bottom = 594;
        rNext.left = 486; rNext.top = 556; rNext.right = 526; rNext.bottom = 594;
        rStop.left = 538; rStop.top = 556; rStop.right = 578; rStop.bottom = 594;
        rShuf.left = 330; rShuf.top = 556; rShuf.right = 370; rShuf.bottom = 594;
        rRep.left  = 590; rRep.top  = 556; rRep.right  = 630; rRep.bottom  = 594;
        rVol.left = 700;  rVol.top = 566; rVol.right = 870; rVol.bottom = 584;
    }
}

static void drawCatScreen(HDC d)
{
    int i;
    RECT bar;

    txtShadow(d, "ShinraMusic", 0, 70, WIN_W, 90, fTitle, RGB(255, 255, 255), DT_CENTER | DT_SINGLELINE);
    bar.left = WIN_W / 2 - 70; bar.right = WIN_W / 2 + 70; bar.top = 139; bar.bottom = 142;
    alphaRect(d, &bar, accent, 255);
    txt(d, "Choisis ta categorie", 0, 150, WIN_W, 28, fMid, RGB(230, 220, 220), DT_CENTER | DT_SINGLELINE);

    for (i = 0; i < ncat; i++)
        card(d, &btn[i], cats[i].name, accent, hot == i);
}

static void drawLock(HDC d, int cx, int cy, COLORREF c)
{
    HPEN p = CreatePen(PS_SOLID, 4, c);
    HBRUSH b = CreateSolidBrush(c);
    HGDIOBJ op = SelectObject(d, p);
    HGDIOBJ ob = SelectObject(d, GetStockObject(NULL_BRUSH));
    RECT k;

    Ellipse(d, cx - 12, cy - 22, cx + 12, cy + 6);
    SelectObject(d, b);
    SelectObject(d, GetStockObject(NULL_PEN));
    RoundRect(d, cx - 18, cy - 4, cx + 18, cy + 22, 8, 8);
    SelectObject(d, op); SelectObject(d, ob);
    DeleteObject(p); DeleteObject(b);

    k.left = cx - 4; k.right = cx + 4; k.top = cy + 3; k.bottom = cy + 11;
    circle(d, &k, RGB(20, 20, 30), RGB(20, 20, 30));
}

static void drawCodeScreen(HDC d)
{
    RECT pn, s, u, k;
    int i, lim, sw, gap, total, x0, y0, caret, w;
    COLORREF edge;

    if (selCat < 0 || selCat >= ncat) return;

    pn.left = 240; pn.top = 100; pn.right = 720; pn.bottom = 520;
    w = pn.right - pn.left;
    panel(d, &pn, mixCol(accent, RGB(0, 0, 0), 35), 140, 2);

    drawLock(d, WIN_W / 2, 150, accent);
    txtShadow(d, cats[selCat].name, pn.left, 188, w, 44, fBig, RGB(255, 255, 255), DT_CENTER | DT_SINGLELINE);
    txt(d, "Entre ton code d'acces", pn.left, 236, w, 26, fMid, RGB(215, 215, 235), DT_CENTER | DT_SINGLELINE);

    lim = (int)strlen(cats[selCat].code);
    gap = 8;
    sw = (440 - (lim - 1) * gap) / lim;
    if (sw > 46) sw = 46;
    total = lim * sw + (lim - 1) * gap;
    x0 = (WIN_W - total) / 2;
    y0 = 280;
    caret = (int)((GetTickCount() / 500) % 2);

    for (i = 0; i < lim; i++) {
        s.left = x0 + i * (sw + gap); s.right = s.left + sw; s.top = y0; s.bottom = y0 + 56;
        alphaRectR(d, &s, accent, i < codeLen ? 80 : 30, 10);
        if (i == codeLen) edge = accent;
        else if (i < codeLen) edge = mixCol(accent, RGB(0, 0, 0), 20);
        else edge = mixCol(accent, RGB(0, 0, 0), 60);
        frame(d, &s, edge, 2, 16);
        if (i < codeLen) {
            int cx = (s.left + s.right) / 2, cy = (s.top + s.bottom) / 2;
            k.left = cx - 6; k.right = cx + 6; k.top = cy - 6; k.bottom = cy + 6;
            circle(d, &k, RGB(255, 255, 255), accent);
        } else if (i == codeLen && caret) {
            u.left = s.left + 8; u.right = s.right - 8; u.top = s.bottom - 11; u.bottom = s.bottom - 8;
            alphaRect(d, &u, accent, 255);
        }
    }

    if (info[0])
        txt(d, info, pn.left, 346, w, 26, fMid, RGB(255, 120, 120), DT_CENTER | DT_SINGLELINE);

    pill(d, &btn[0], "VALIDER", accent, 235, hot == 0);
    pill(d, &btn[1], "RETOUR", RGB(0, 0, 0), 100, hot == 1);
    frame(d, &btn[1], mixCol(accent, RGB(0, 0, 0), hot == 1 ? 0 : 55), 2, 46);
}

static void drawNow(HDC d)
{
    int cx = (rNow.left + rNow.right) / 2, cy = rNow.top + 100;
    int i, active = (cur >= 0 && cur < ntracks);
    int live = (isOpen && playing && !paused);
    char buf[300], nfo[96];
    RECT dr, lab;
    COLORREF edge = active ? accent : RGB(90, 90, 110);

    panel(d, &rNow, accent, 165, 2);

    dr.left = cx - 80; dr.top = cy - 80; dr.right = cx + 80; dr.bottom = cy + 80;
    circle(d, &dr, RGB(14, 14, 24), edge);
    ring(d, cx, cy, 66, RGB(42, 42, 60), 1);
    ring(d, cx, cy, 52, RGB(36, 36, 52), 1);
    lab.left = cx - 34; lab.top = cy - 34; lab.right = cx + 34; lab.bottom = cy + 34;
    circle(d, &lab, active ? accent : RGB(70, 70, 90), RGB(14, 14, 24));
    if (active) {
        char c = tracks[cur].name[0];
        if (c >= 'a' && c <= 'z') c -= 32;
        if (!((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))) c = '#';
        nfo[0] = c; nfo[1] = 0;
        txt(d, nfo, lab.left, lab.top, 68, 68, fBig, RGB(255, 255, 255), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    if (active) strcpy(buf, tracks[cur].name);
    else strcpy(buf, "Aucun titre");
    txt(d, buf, rNow.left + 14, cy + 92, rNow.right - rNow.left - 28, 58,
        fMidB, RGB(255, 255, 255), DT_CENTER | DT_WORDBREAK | DT_END_ELLIPSIS);

    if (active) sprintf(nfo, "%d / %d%s%s", cur + 1, ntracks,
                        shuffle ? "     ALEATOIRE" : "", repeat ? "     REPETE" : "");
    else nfo[0] = 0;
    txt(d, nfo, rNow.left + 10, cy + 152, rNow.right - rNow.left - 20, 22,
        fSm, RGB(200, 200, 220), DT_CENTER | DT_SINGLELINE);

    for (i = 0; i < 16; i++) {
        unsigned v = (unsigned)(GetTickCount() / 140 + i * 37) * 2654435761u;
        int h = live ? 5 + (int)((v >> 24) % 40) : 4;
        RECT b;
        b.left = cx - 141 + i * 18; b.right = b.left + 12;
        b.bottom = rNow.bottom - 12; b.top = b.bottom - h;
        alphaRect(d, &b, accent, live ? 235 : 90);
    }
}

static void drawList(HDC d)
{
    int r, len = 0, pos = 0;
    char buf[300], num[16], t1[16], t2[16];
    RECT row;

    txtShadow(d, catName, 60, 38, 640, 44, fBig, accent, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    alphaRectR(d, &rBack, RGB(255, 255, 255), hot == H_BACK ? 85 : 35, 10);
    txt(d, "< RETOUR", rBack.left, rBack.top, rBack.right - rBack.left, rBack.bottom - rBack.top,
        fMidB, RGB(255, 255, 255), DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    panel(d, &rList, accent, 165, 2);

    if (!ntracks) {
        const char *m = plState == 0 ? "Chargement de la playlist..." :
                        (plErr[0] ? plErr : "Aucun titre.");
        txt(d, m, rList.left + 20, rList.top, rList.right - rList.left - 40, rList.bottom - rList.top,
            fMid, RGB(220, 220, 235), DT_CENTER | DT_VCENTER | DT_WORDBREAK);
    } else {
        for (r = 0; r < VIS; r++) {
            int idx = scroll + r;
            if (idx >= ntracks) break;
            row.left = rList.left + 10; row.right = rList.right - 20;
            row.top = rList.top + 6 + r * ROWH; row.bottom = row.top + ROWH - 4;

            if (idx == cur) {
                RECT st;
                alphaRect(d, &row, accent, 100);
                st.left = row.left; st.right = row.left + 4; st.top = row.top; st.bottom = row.bottom;
                alphaRect(d, &st, accent, 255);
            } else if (hot == 100 + idx) alphaRect(d, &row, RGB(255, 255, 255), 45);

            sprintf(num, "%02d", idx + 1);
            txt(d, num, row.left + 12, row.top, 40, ROWH - 4, fSm,
                idx == cur ? RGB(255, 255, 255) : RGB(170, 170, 195), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            txt(d, tracks[idx].name, row.left + 56, row.top, row.right - row.left - 70, ROWH - 4, fMid,
                RGB(255, 255, 255), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            if (idx == cur && playing && !paused) {
                POINT p[3];
                HBRUSH b = CreateSolidBrush(RGB(255, 255, 255));
                HGDIOBJ ob = SelectObject(d, b), op = SelectObject(d, GetStockObject(NULL_PEN));
                p[0].x = row.right - 26; p[0].y = row.top + 7;
                p[1].x = row.right - 26; p[1].y = row.top + 23;
                p[2].x = row.right - 12; p[2].y = row.top + 15;
                Polygon(d, p, 3);
                SelectObject(d, ob); SelectObject(d, op); DeleteObject(b);
            }
        }
        if (ntracks > VIS) {
            RECT tr, th;
            int th_h = (rList.bottom - rList.top - 24) * VIS / ntracks;
            int th_y;
            if (th_h < 24) th_h = 24;
            th_y = rList.top + 12 + (rList.bottom - rList.top - 24 - th_h) * scroll / (ntracks - VIS);
            tr.left = rList.right - 12; tr.right = rList.right - 8; tr.top = rList.top + 12; tr.bottom = rList.bottom - 12;
            th.left = tr.left; th.right = tr.right; th.top = th_y; th.bottom = th_y + th_h;
            alphaRect(d, &tr, RGB(255, 255, 255), 40);
            alphaRect(d, &th, accent, 230);
        }
    }

    drawNow(d);

    panel(d, &rPlayer, accent, 190, 2);

    if (playErr[0]) strcpy(buf, playErr);
    else if (loading) strcpy(buf, "Chargement du titre...");
    else if (cur >= 0 && cur < ntracks) strcpy(buf, tracks[cur].name);
    else strcpy(buf, "Choisis un titre dans la liste");
    txt(d, buf, 90, 496, 780, 30, fMid, RGB(255, 255, 255), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

    if (isOpen) {
        len = mciInt("status song length");
        pos = mciInt("status song position");
    }
    {
        RECT bar, fill;
        int w = rSeek.right - rSeek.left;
        int fw = len > 0 ? (int)((double)pos / len * w) : 0;
        bar.left = rSeek.left; bar.right = rSeek.right; bar.top = rSeek.top + 7; bar.bottom = rSeek.top + 13;
        alphaRect(d, &bar, RGB(255, 255, 255), 70);
        fill = bar; fill.right = bar.left + fw;
        alphaRect(d, &fill, accent, 255);
        if (isOpen) {
            RECT k; k.left = bar.left + fw - 7; k.right = bar.left + fw + 7; k.top = rSeek.top + 3; k.bottom = rSeek.top + 17;
            circle(d, &k, RGB(255, 255, 255), accent);
        }
    }
    fmtTime(pos, t1); fmtTime(len, t2);
    txt(d, t1, 66, 531, 60, 26, fSm, RGB(215, 215, 235), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
    txt(d, t2, 836, 531, 60, 26, fSm, RGB(215, 215, 235), DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    circle(d, &rPrev, RGB(25, 25, 45), hot == H_PREV ? RGB(255, 255, 255) : RGB(140, 140, 170));
    icon(d, 0, &rPrev, RGB(255, 255, 255));
    circle(d, &rPlay, accent, hot == H_PLAY ? RGB(255, 255, 255) : accent);
    icon(d, (playing && !paused) ? 2 : 1, &rPlay, RGB(255, 255, 255));
    circle(d, &rNext, RGB(25, 25, 45), hot == H_NEXT ? RGB(255, 255, 255) : RGB(140, 140, 170));
    icon(d, 3, &rNext, RGB(255, 255, 255));
    circle(d, &rStop, RGB(25, 25, 45), hot == H_STOP ? RGB(255, 255, 255) : RGB(140, 140, 170));
    icon(d, 4, &rStop, RGB(255, 255, 255));

    circle(d, &rShuf, shuffle ? accent : RGB(25, 25, 45), hot == H_SHUF ? RGB(255, 255, 255) : RGB(140, 140, 170));
    txt(d, "S", rShuf.left, rShuf.top, 40, 38, fSm, RGB(255, 255, 255), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    circle(d, &rRep, repeat ? accent : RGB(25, 25, 45), hot == H_REP ? RGB(255, 255, 255) : RGB(140, 140, 170));
    txt(d, "R", rRep.left, rRep.top, 40, 38, fSm, RGB(255, 255, 255), DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    txt(d, "VOL", 650, 562, 45, 26, fSm, RGB(215, 215, 235), DT_LEFT | DT_SINGLELINE);
    {
        RECT bar, fill;
        int w = rVol.right - rVol.left;
        int fw = volume * w / 1000;
        RECT k;
        bar.left = rVol.left; bar.right = rVol.right; bar.top = rVol.top + 6; bar.bottom = rVol.top + 12;
        alphaRect(d, &bar, RGB(255, 255, 255), 70);
        fill = bar; fill.right = bar.left + fw;
        alphaRect(d, &fill, accent, 255);
        k.left = bar.left + fw - 6; k.right = bar.left + fw + 6; k.top = rVol.top + 3; k.bottom = rVol.top + 15;
        circle(d, &k, RGB(255, 255, 255), accent);
    }
}

static void drawTitleBar(HDC d, int w, int h)
{
    RECT g;
    HPEN p, op;
    HGDIOBJ ob;
    COLORREF dim;
    int y, cx, cy = TITLE_H / 2;

    (void)g; (void)y;

    txtShadow(d, "ShinraMusic", 20, 0, 260, TITLE_H, fMidB, RGB(255, 255, 255), DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    alphaRectR(d, &rMin, RGB(255, 255, 255), hot == H_MIN ? 85 : 30, 8);
    if (hot == H_CLOSE) alphaRectR(d, &rClose, RGB(232, 17, 35), 235, 8);
    else                alphaRectR(d, &rClose, RGB(255, 255, 255), 30, 8);

    p = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
    op = SelectObject(d, p);
    cx = (rMin.left + rMin.right) / 2;
    MoveToEx(d, cx - 5, cy + 3, NULL); LineTo(d, cx + 6, cy + 3);
    cx = (rClose.left + rClose.right) / 2;
    MoveToEx(d, cx - 5, cy - 5, NULL); LineTo(d, cx + 6, cy + 6);
    MoveToEx(d, cx + 5, cy - 5, NULL); LineTo(d, cx - 6, cy + 6);
    SelectObject(d, op);
    DeleteObject(p);

    dim = RGB(GetRValue(accent) * 55 / 100, GetGValue(accent) * 55 / 100, GetBValue(accent) * 55 / 100);
    p = CreatePen(PS_SOLID, 1, dim);
    op = SelectObject(d, p);
    ob = SelectObject(d, GetStockObject(NULL_BRUSH));
    RoundRect(d, 0, 0, w - 1, h - 1, 24, 24);
    SelectObject(d, op); SelectObject(d, ob);
    DeleteObject(p);
}

static void draw(HDC d, int w, int h)
{
    RECT full;
    full.left = 0; full.top = 0; full.right = w; full.bottom = h;
    SetBkMode(d, TRANSPARENT);
    drawBg(d, w, h);
    alphaRect(d, &full, RGB(0, 0, 0), 100);
    layout();
    switch (screen) {
    case SCR_CATS: drawCatScreen(d);  break;
    case SCR_CODE: drawCodeScreen(d); break;
    case SCR_LIST: drawList(d);       break;
    }
    drawTitleBar(d, w, h);
}

static COLORREF bgAccent(void)
{
    BITMAPINFO bi;
    HDC scr, sdc, ddc;
    HBITMAP db;
    unsigned int *px = NULL;
    int i, n = 64 * 40;
    double sr = 0, sg = 0, sb = 0, sw = 0, mx;
    COLORREF res = RGB(230, 230, 245);

    if (!bg || bgW <= 0 || bgH <= 0) return res;

    memset(&bi, 0, sizeof bi);
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = 64;
    bi.bmiHeader.biHeight = -40;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    scr = GetDC(NULL);
    sdc = CreateCompatibleDC(scr);
    ddc = CreateCompatibleDC(scr);
    db = CreateDIBSection(scr, &bi, DIB_RGB_COLORS, (void **)&px, NULL, 0);

    if (sdc && ddc && db && px) {
        HGDIOBJ so, dobj;
        dobj = SelectObject(ddc, db);
        so = SelectObject(sdc, bg);
        SetStretchBltMode(ddc, HALFTONE);
        SetBrushOrgEx(ddc, 0, 0, NULL);
        StretchBlt(ddc, 0, 0, 64, 40, sdc, 0, 0, bgW, bgH, SRCCOPY);
        SelectObject(sdc, so);
        SelectObject(ddc, dobj);
        GdiFlush();

        for (i = 0; i < n; i++) {
            unsigned int p = px[i];
            int r = (p >> 16) & 255, g = (p >> 8) & 255, b = p & 255;
            int hi = r > g ? (r > b ? r : b) : (g > b ? g : b);
            int lo = r < g ? (r < b ? r : b) : (g < b ? g : b);
            double w;
            if (hi < 30) continue;
            w = (double)(hi - lo);
            w = w * w + 1.0;
            sr += r * w; sg += g * w; sb += b * w; sw += w;
        }
        if (sw > 0) {
            sr /= sw; sg /= sw; sb /= sw;
            mx = sr > sg ? (sr > sb ? sr : sb) : (sg > sb ? sg : sb);
            if (mx >= 1) {
                sr = sr * 225.0 / mx * 0.92 + 20.0;
                sg = sg * 225.0 / mx * 0.92 + 20.0;
                sb = sb * 225.0 / mx * 0.92 + 20.0;
                res = RGB((int)sr, (int)sg, (int)sb);
            }
        }
    }

    if (db) DeleteObject(db);
    if (sdc) DeleteDC(sdc);
    if (ddc) DeleteDC(ddc);
    ReleaseDC(NULL, scr);
    return res;
}

static void loadBg(const char *path)
{
    HBITMAP nb;
    BITMAP bm;

    nb = (HBITMAP)LoadImageA(NULL, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (!nb) return;
    memset(&bm, 0, sizeof bm);
    GetObjectA(nb, sizeof(BITMAP), &bm);
    if (bm.bmWidth <= 0 || bm.bmHeight <= 0) { DeleteObject(nb); return; }
    if (bg) DeleteObject(bg);
    bg = nb;
    bgW = bm.bmWidth; bgH = bm.bmHeight;
    accent = bgAccent();
}

typedef BOOL (WINAPI *CLIPOPENFN)(HWND);
typedef HANDLE (WINAPI *CLIPGETFN)(UINT);
typedef BOOL (WINAPI *CLIPCLOSEFN)(void);
typedef void *(WINAPI *GLOCKFN)(HANDLE);
typedef BOOL (WINAPI *GUNLOCKFN)(HANDLE);

static void pasteCode(void)
{
    HMODULE u, k;
    CLIPOPENFN pOpen;
    CLIPGETFN pGet;
    CLIPCLOSEFN pClose;
    GLOCKFN pLock;
    GUNLOCKFN pUnlock;
    HANDLE hm;
    int lim;

    if (selCat < 0 || selCat >= ncat || catLocked[selCat]) return;
    u = LoadLibraryA("user32.dll");
    k = LoadLibraryA("kernel32.dll");
    if (!u || !k) return;
    pOpen   = (CLIPOPENFN)GetProcAddress(u, "OpenClipboard");
    pGet    = (CLIPGETFN)GetProcAddress(u, "GetClipboardData");
    pClose  = (CLIPCLOSEFN)GetProcAddress(u, "CloseClipboard");
    pLock   = (GLOCKFN)GetProcAddress(k, "GlobalLock");
    pUnlock = (GUNLOCKFN)GetProcAddress(k, "GlobalUnlock");
    if (!pOpen || !pGet || !pClose || !pLock || !pUnlock) return;

    lim = (int)strlen(cats[selCat].code);
    if (!pOpen(hwnd)) return;
    hm = pGet(CLIP_TEXT);
    if (hm) {
        const char *t = (const char *)pLock(hm);
        if (t) {
            codeLen = 0;
            codeInput[0] = 0;
            for (; *t && codeLen < lim; t++)
                if ((unsigned char)*t > 32 && (unsigned char)*t < 127)
                    codeInput[codeLen++] = *t;
            codeInput[codeLen] = 0;
            info[0] = 0;
            pUnlock(hm);
        }
    }
    pClose();
}

static int PT(const RECT *r, int x, int y)
{
    return x >= r->left && x < r->right && y >= r->top && y < r->bottom;
}

static int hitTest(int x, int y)
{
    int i;
    layout();
    if (PT(&rMin, x, y))   return H_MIN;
    if (PT(&rClose, x, y)) return H_CLOSE;
    for (i = 0; i < nbtn; i++) if (PT(&btn[i], x, y)) return i;
    if (screen == SCR_LIST) {
        if (PT(&rBack, x, y)) return H_BACK;
        if (PT(&rPrev, x, y)) return H_PREV;
        if (PT(&rPlay, x, y)) return H_PLAY;
        if (PT(&rNext, x, y)) return H_NEXT;
        if (PT(&rStop, x, y)) return H_STOP;
        if (PT(&rShuf, x, y)) return H_SHUF;
        if (PT(&rRep,  x, y)) return H_REP;
        if (PT(&rSeek, x, y)) return H_SEEK;
        if (PT(&rVol,  x, y)) return H_VOL;
        if (PT(&rList, x, y)) {
            int row = (y - rList.top - 6) / ROWH;
            int idx = scroll + row;
            if (row >= 0 && row < VIS && idx < ntracks) return 100 + idx;
        }
    }
    return H_NONE;
}

static void volFromX(int x)
{
    setVolume((x - rVol.left) * 1000 / (rVol.right - rVol.left));
}

static void submitCode(void)
{
    const char *good;
    if (selCat < 0 || selCat >= ncat) return;
    if (catLocked[selCat]) { strcpy(info, "Trop d'essais : relance l'application"); return; }
    good = cats[selCat].code;
    if (codeLen != (int)strlen(good)) { strcpy(info, "Code incomplet"); return; }
    if (plBusy) { strcpy(info, "Chargement en cours, patiente un instant..."); return; }
    if (strcmp(codeInput, good) == 0) {
        info[0] = 0;
        strcpy(catName, cats[selCat].name);
        codeLen = 0; codeInput[0] = 0;
        screen = SCR_LIST;
        startPlaylist();
    } else {
        catTries[selCat]++;
        codeLen = 0; codeInput[0] = 0;
        if (catTries[selCat] >= 5) {
            catLocked[selCat] = 1;
            strcpy(info, "Trop d'essais : relance l'application");
        } else {
            strcpy(info, "Code incorrect");
        }
    }
}

static void onClick(int id, int x)
{
    if (id == H_NONE) return;
    if (id == H_CLOSE) { PostMessageA(hwnd, WM_CLOSE, 0, 0); return; }
    if (id == H_MIN)   { ShowWindow(hwnd, SW_MINIMIZE); return; }

    if (screen == SCR_CATS) {
        if (id >= 0 && id < ncat) enterCat(id);
    } else if (screen == SCR_CODE) {
        if (id == 0) submitCode();
        else if (id == 1) goBack();
    } else if (screen == SCR_LIST) {
        if (id == H_BACK) goBack();
        else if (id >= 100 && id < 100 + MAX_TR) playTrack(id - 100);
        else if (id == H_PLAY) togglePlay();
        else if (id == H_PREV) prevTrack();
        else if (id == H_NEXT) nextTrack();
        else if (id == H_STOP) { stopTrack(); }
        else if (id == H_SHUF) shuffle = !shuffle;
        else if (id == H_REP)  repeat = !repeat;
        else if (id == H_SEEK) seekTo(x);
        else if (id == H_VOL)  { drag = H_VOL; volFromX(x); }
    }
}

static LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM wp, LPARAM lp)
{
    switch (m) {
    case WM_ERASEBKGND:
        return 1;

    case WM_NCHITTEST: {
        POINT pt;
        pt.x = (short)LOWORD(lp); pt.y = (short)HIWORD(lp);
        ScreenToClient(h, &pt);
        if (pt.y >= 0 && pt.y < TITLE_H && pt.x >= 0 && pt.x < WIN_W - 92) return HTCAPTION;
        return HTCLIENT;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(h, &ps);
        RECT rc;
        GetClientRect(h, &rc);
        if (ensureBackbuffer(hdc, rc.right, rc.bottom)) {
            draw(memDC, rc.right, rc.bottom);
            GdiFlush();
            BitBlt(hdc, 0, 0, rc.right, rc.bottom, memDC, 0, 0, SRCCOPY);
        }
        EndPaint(h, &ps);
        return 0;
    }

    case WM_MOUSEMOVE: {
        int x = (short)LOWORD(lp), y = (short)HIWORD(lp);
        int nh = hitTest(x, y);
        if (drag == H_VOL) { volFromX(x); InvalidateRect(h, NULL, FALSE); }
        if (nh != hot) { hot = nh; InvalidateRect(h, NULL, FALSE); }
        return 0;
    }

    case WM_SETCURSOR:
        if (LOWORD(lp) == HTCLIENT && hot != H_NONE) {
            SetCursor(LoadCursorA(NULL, IDC_HAND));
            return TRUE;
        }
        break;

    case WM_LBUTTONDOWN: {
        int x = (short)LOWORD(lp), y = (short)HIWORD(lp);
        SetCapture(h);
        onClick(hitTest(x, y), x);
        hot = hitTest(x, y);
        InvalidateRect(h, NULL, FALSE);
        return 0;
    }

    case WM_LBUTTONUP:
        drag = H_NONE;
        ReleaseCapture();
        return 0;

    case WM_MOUSEWHEEL:
        if (screen == SCR_LIST && ntracks > VIS) {
            int dl = (short)HIWORD(wp);
            scroll -= dl / 120;
            if (scroll < 0) scroll = 0;
            if (scroll > ntracks - VIS) scroll = ntracks - VIS;
            InvalidateRect(h, NULL, FALSE);
        }
        return 0;

    case WM_CHAR:
        if (screen == SCR_CATS) {
            char c = (char)wp;
            if (c >= '1' && c <= '9') enterCat(c - '1');
            InvalidateRect(h, NULL, FALSE);
        } else if (screen == SCR_CODE && selCat >= 0 && selCat < ncat) {
            char c = (char)wp;
            int lim = (int)strlen(cats[selCat].code);
            if (c == 27) goBack();
            else if (!catLocked[selCat]) {
                if (c == '\r') submitCode();
                else if (c == '\b') { if (codeLen > 0) codeInput[--codeLen] = 0; }
                else if (codeLen < lim && c > 32 && c < 127) {
                    codeInput[codeLen++] = c;
                    codeInput[codeLen] = 0;
                    info[0] = 0;
                }
            }
            InvalidateRect(h, NULL, FALSE);
        }
        return 0;

    case WM_KEYDOWN:
        if (screen == SCR_CODE) {
            if ((wp == 'V' && (GetKeyState(VK_CONTROL) & 0x8000)) ||
                (wp == VK_INSERT && (GetKeyState(VK_SHIFT) & 0x8000))) {
                pasteCode();
                InvalidateRect(h, NULL, FALSE);
            }
        } else if (screen == SCR_LIST) {
            if (wp == VK_SPACE) togglePlay();
            else if (wp == VK_RIGHT) nextTrack();
            else if (wp == VK_LEFT) prevTrack();
            else if (wp == VK_UP) setVolume(volume + 50);
            else if (wp == VK_DOWN) setVolume(volume - 50);
            else if (wp == 'S') shuffle = !shuffle;
            else if (wp == 'R') repeat = !repeat;
            else if (wp == VK_ESCAPE) goBack();
            InvalidateRect(h, NULL, FALSE);
        }
        return 0;

    case WM_DL_DONE:
        if ((int)lp == reqGen && loading) {
            loading = 0;
            if (wp && tmpPath[0]) startPlayback();
            else {
                strcpy(playErr, "T\xe9l\xe9" "chargement impossible (lien ou connexion ?)");
                if (tmpPath[0]) { remove(tmpPath); tmpPath[0] = 0; }
            }
        }
        InvalidateRect(h, NULL, FALSE);
        return 0;

    case WM_UPD_READY:
        if (screen == SCR_CATS && !isOpen) applyUpdate();
        else updPending = 1;
        return 0;

    case WM_BG_DONE:
        if (wp && bgTmp[0]) loadBg(bgTmp);
        if (bgTmp[0]) remove(bgTmp);
        InvalidateRect(h, NULL, FALSE);
        return 0;

    case WM_CFG_DONE:
        if (cfgN > 0) {
            int i;
            for (i = 0; i < cfgN; i++) cats[i] = cfgCats[i];
            ncat = cfgN;
            if (screen == SCR_CODE) {
                screen = SCR_CATS;
                codeLen = 0; codeInput[0] = 0;
                info[0] = 0;
            }
        }
        InvalidateRect(h, NULL, FALSE);
        return 0;

    case WM_PL_DONE:
        InvalidateRect(h, NULL, FALSE);
        return 0;

    case WM_TIMER:
        if (screen == SCR_LIST && isOpen && playing && !paused) {
            char b[32];
            mci("status song mode", b, sizeof b);
            if (!strcmp(b, "stopped")) {
                if (repeat) playTrack(cur);
                else nextTrack();
            }
        }
        if (screen == SCR_LIST || screen == SCR_CODE) InvalidateRect(h, NULL, FALSE);
        return 0;

    case WM_DESTROY:
        stopTrack();
        if (bgTmp[0]) remove(bgTmp);
        if (updTmp[0]) remove(updTmp);
        if (memBmp) DeleteObject(memBmp);
        if (memDC) DeleteDC(memDC);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(h, m, wp, lp);
}

int WINAPI WinMain(HINSTANCE hi, HINSTANCE hp, LPSTR cl, int sw)
{
    WNDCLASSA wc;
    MSG ms;
    RECT r;
    HMODULE m;
    HANDLE th;
    char exe[MAX_PATH];
    DWORD style = WS_POPUP | WS_SYSMENU | WS_MINIMIZEBOX;
    char *slash;

    (void)hp; (void)cl; (void)sw;

    SetUnhandledExceptionFilter(crashHandler);

    GetModuleFileNameA(NULL, exe, MAX_PATH);
    strcpy(selfPath, exe);
    slash = strrchr(exe, '\\');
    if (slash) { *slash = 0; SetCurrentDirectoryA(exe); }

    m = LoadLibraryA("winmm.dll");
    if (m) pMci = (MCIFN)GetProcAddress(m, "mciSendStringA");

    accent = RGB(230, 230, 245);
    loadBg("fond.bmp");

    fTitle = mkfont(54, 1, "Segoe UI");
    fBig   = mkfont(30, 1, "Segoe UI");
    fMid   = mkfont(19, 0, "Segoe UI");
    fMidB  = mkfont(21, 1, "Segoe UI");
    fSm    = mkfont(15, 0, "Segoe UI");
    fMono  = mkfont(40, 1, "Consolas");

    srand((unsigned)GetTickCount());
    initCats();

    icoBig = (HICON)LoadImageA(hi, MAKEINTRESOURCE(1), IMAGE_ICON,
                               GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), 0);
    if (!icoBig)
        icoBig = (HICON)LoadImageA(NULL, "icone.ico", IMAGE_ICON,
                                   GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_LOADFROMFILE);
    icoSm  = (HICON)LoadImageA(hi, MAKEINTRESOURCE(1), IMAGE_ICON,
                               GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
    if (!icoSm)
        icoSm = (HICON)LoadImageA(NULL, "icone.ico", IMAGE_ICON,
                                  GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_LOADFROMFILE);

    memset(&wc, 0, sizeof wc);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hi;
    wc.hIcon         = icoBig;
    wc.hCursor       = LoadCursorA(NULL, IDC_ARROW);
    wc.lpszClassName = "ShinraMusicWnd";
    if (!RegisterClassA(&wc)) { MessageBoxA(NULL, "RegisterClass a echoue", "ShinraMusic", MB_OK | MB_ICONERROR); return 1; }

    r.left = 0; r.top = 0; r.right = WIN_W; r.bottom = WIN_H;
    AdjustWindowRect(&r, style, FALSE);
    hwnd = CreateWindowA("ShinraMusicWnd", "ShinraMusic", style,
                         (GetSystemMetrics(SM_CXSCREEN) - (r.right - r.left)) / 2,
                         (GetSystemMetrics(SM_CYSCREEN) - (r.bottom - r.top)) / 2,
                         r.right - r.left, r.bottom - r.top, NULL, NULL, hi, NULL);
    if (!hwnd) {
        char b[96];
        sprintf(b, "CreateWindow a echoue, erreur %lu", (unsigned long)GetLastError());
        MessageBoxA(NULL, b, "ShinraMusic", MB_OK | MB_ICONERROR);
        return 1;
    }
    if (icoBig) SendMessageA(hwnd, WM_SETICON, ICON_BIG, (LPARAM)icoBig);
    if (icoSm)  SendMessageA(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icoSm);
    SetWindowRgn(hwnd, CreateRoundRectRgn(0, 0, r.right - r.left + 1, r.bottom - r.top + 1, 24, 24), TRUE);
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    SetTimer(hwnd, 1, 150, NULL);

    if (!strstr(GH_USER, "TONPSEUDO")) {
        th = CreateThread(NULL, 0, cfgThread, NULL, 0, NULL);
        if (th) CloseHandle(th);
        GetTempPathA(MAX_PATH - 40, bgTmp);
        sprintf(bgTmp + strlen(bgTmp), "sm_bg_%lu.bmp", (unsigned long)GetCurrentProcessId());
        th = CreateThread(NULL, 0, bgThread, NULL, 0, NULL);
        if (th) CloseHandle(th);
        GetTempPathA(MAX_PATH - 40, updTmp);
        sprintf(updTmp + strlen(updTmp), "sm_upd_%lu.exe", (unsigned long)GetCurrentProcessId());
        th = CreateThread(NULL, 0, updThread, NULL, 0, NULL);
        if (th) CloseHandle(th);
    }

    while (GetMessageA(&ms, NULL, 0, 0) > 0) {
        TranslateMessage(&ms);
        DispatchMessageA(&ms);
    }
    return 0;
}
