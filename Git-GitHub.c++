#include <QtWidgets>
#include <QDesktopServices>
#include <functional>
#include <vector>

static const QString YT = "https://www.youtube.com/@GameDeveloperAnup";
static void openYT() { QDesktopServices::openUrl(QUrl(YT)); }
static QString U(const char *s) { return QString::fromUtf8(s); }

struct Lesson { const char *title, *sub, *body; };

// ---- Baaki lessons yahin isi format me paste karo ----
static const std::vector<Lesson> LESSONS = {
{"What is Git?","Version Control Basics",
 "Git is a version control system. It records changes to your files over time so you can go back to any earlier version.\n"
 "# Commit\nEvery saved point in history is called a commit. Think of commits as a timeline: A -> B -> C.\n"
 "# Why use Git\n- Clear history of every change\n- Safe backup points\n- Easy teamwork\n- Undo mistakes safely\n"
 "# Simple loop\n- Edit files\n- Stage them\n- Commit them\n- Push to GitHub"},
{"Installing Git","Windows, macOS, Linux",
 "Download from git-scm.com or use a package manager.\n"
 "# Windows\n$ winget install --id Git.Git -e --source winget\n"
 "# macOS\n$ xcode-select --install\n$ brew install git\n"
 "# Linux (Debian/Ubuntu)\n$ sudo apt update\n$ sudo apt install git\n"
 "# Verify\n$ git --version\n"
 "You can use Git from the command line, VS Code or GitHub Desktop. All use the same Git underneath."},
{"The Big Four Commands","status, add, commit, log",
 "# git status\nShows the current state of the repo.\n"
 "# git add\n$ git add README.md\n$ git add .\n$ git add *.py\n"
 "# git commit\n$ git commit -m \"add readme file\"\n$ git commit -am \"fix typo\"\n"
 "-am only works for files Git already tracks.\n"
 "# git log\n$ git log --oneline\n$ git log --oneline --graph --all"},
{"Branching","Independent lines of work",
 "A branch is an independent line of development. main is the default.\n"
 "# Create and switch\n$ git branch feature/search\n$ git switch feature/search\n$ git switch -c feature/search\n"
 "# View\n$ git branch\n$ git branch -a\n$ git branch --show-current\n"
 "# Graph\n$ git log --oneline --graph --all\n"
 "Branches help build features in isolation, fix bugs safely and experiment."},
{"Git Internals","How Git stores data",
 "Under the hood Git is a simple content database. Understanding it removes most of the mystery from commands like reset and rebase.\n"
 "# The .git folder\nEverything lives in the hidden .git folder: objects, branch pointers, HEAD and config.\n"
 "# Four object types\n- Blob: the contents of one file\n- Tree: a folder listing\n- Commit: a snapshot pointing to one tree plus its parent\n- Tag: a named pointer to a commit\n"
 "# Look inside\n$ git cat-file -t a1b2c3d\n$ git cat-file -p HEAD\n$ git ls-files\n$ git rev-parse HEAD\n"
 "# Branches are tiny\nA branch is just a file containing one commit hash. That is why creating a branch is instant.\n$ cat .git/HEAD"},
};

// ---------- helpers ----------
static QLabel *mk(const QString &t, const QString &css, bool wrap = true) {
    auto *l = new QLabel(t); l->setWordWrap(wrap); l->setStyleSheet(css + "background:transparent;"); return l;
}
static QPushButton *btn(const QString &t, const QString &bg, const QString &fg, const QString &bd) {
    auto *b = new QPushButton(t); b->setCursor(Qt::PointingHandCursor); b->setMinimumHeight(50);
    b->setStyleSheet(QString("QPushButton{background:%1;color:%2;border:2px solid %3;border-radius:14px;"
                             "font-size:16px;font-weight:700;padding:0 16px;}").arg(bg, fg, bd));
    return b;
}
static QScrollArea *makeScroll(QWidget *inner) {
    auto *s = new QScrollArea; s->setWidgetResizable(true); s->setFrameShape(QFrame::NoFrame);
    s->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); s->setWidget(inner);
    QScroller::grabGesture(s->viewport(), QScroller::LeftMouseButtonGesture);
    return s;
}

// Clickable card (no moc needed)
class Clickable : public QFrame {
public:
    std::function<void()> onClick;
    using QFrame::QFrame;
protected:
    void mouseReleaseEvent(QMouseEvent *e) override {
        if (rect().contains(e->position().toPoint()) && onClick) onClick();
    }
};

// Mac-style terminal window: 3 colored circles + title
class MacWindow : public QWidget {
public:
    explicit MacWindow(const QString &code, QWidget *p = nullptr) : QWidget(p) {
        auto *lay = new QVBoxLayout(this); lay->setContentsMargins(14, 46, 14, 14);
        auto *lbl = new QLabel; lbl->setTextFormat(Qt::RichText); lbl->setWordWrap(true);
        lbl->setTextInteractionFlags(Qt::TextSelectableByMouse);
        QString h = code.toHtmlEscaped();
        h.replace(QRegularExpression("\\bgit\\b"), "<span style='color:#ff7a93;font-weight:600'>git</span>");
        lbl->setText("<div style='white-space:pre-wrap'>" + h + "</div>");
        lbl->setStyleSheet("color:#e8eaf6;background:transparent;font-size:13px;"
                           "font-family:Menlo,Consolas,'Courier New',monospace;");
        lay->addWidget(lbl);
    }
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this); p.setRenderHint(QPainter::Antialiasing);
        QPainterPath path; path.addRoundedRect(QRectF(rect()), 12, 12); p.setClipPath(path);
        p.fillRect(rect(), QColor("#1e1f2e"));
        p.fillRect(0, 0, width(), 34, QColor("#2b2d42"));
        const char *cols[3] = {"#ff5f57", "#febc2e", "#28c840"};
        p.setPen(Qt::NoPen);
        for (int i = 0; i < 3; i++) { p.setBrush(QColor(cols[i])); p.drawEllipse(QPointF(18 + i * 20, 17), 6, 6); }
        p.setPen(QColor("#9aa0b4"));
        p.drawText(QRect(0, 0, width(), 34), Qt::AlignCenter, U("Terminal — zsh"));
    }
};

// Dark home background with glows
class HomeBg : public QWidget {
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this); p.fillRect(rect(), QColor("#080c14"));
        QRadialGradient a(width() * .8, height() * .1, width());
        a.setColorAt(0, QColor(59, 29, 90, 110)); a.setColorAt(1, QColor(59, 29, 90, 0)); p.fillRect(rect(), a);
        QRadialGradient b(width() * .1, height() * .9, width());
        b.setColorAt(0, QColor(240, 78, 47, 60)); b.setColorAt(1, QColor(240, 78, 47, 0)); p.fillRect(rect(), b);
    }
};

// ---------- main window ----------
class Win : public QMainWindow {
    enum { HOME, LIST, LESSON, SOON, ABOUT };
    static const int DW = 280;
    QStackedWidget *stack; QWidget *bar; QLabel *barTitle; QPushButton *backBtn;
    QWidget *lessonBox; QVBoxLayout *lessonLay; QScrollArea *lessonScroll;
    QFrame *drawer = nullptr; QWidget *dim = nullptr; bool drawerOpen = false; int curPage = HOME;

public:
    Win() {
        auto *central = new QWidget; setCentralWidget(central);
        auto *root = new QVBoxLayout(central); root->setContentsMargins(0, 0, 0, 0); root->setSpacing(0);
        // top bar
        bar = new QWidget; bar->setObjectName("bar"); bar->setAttribute(Qt::WA_StyledBackground);
        bar->setStyleSheet("QWidget#bar{background:#fafafa;border-bottom:1px solid #e6e8ec;}");
        auto *bl = new QHBoxLayout(bar); bl->setContentsMargins(12, 10, 12, 10);
        auto *burger = new QPushButton(U("☰")); burger->setFlat(true);
        burger->setStyleSheet("QPushButton{font-size:26px;border:0;color:#141a2a;padding:0 8px;}");
        connect(burger, &QPushButton::clicked, [this] { openDrawer(true); });
        barTitle = mk("", "font-size:17px;font-weight:700;color:#141a2a;", false);
        backBtn = new QPushButton("Lessons"); backBtn->setFlat(true);
        backBtn->setStyleSheet("QPushButton{color:#f04e2f;font-weight:700;border:0;font-size:15px;}");
        connect(backBtn, &QPushButton::clicked, [this] { showList(); });
        bl->addWidget(burger); bl->addWidget(barTitle, 1); bl->addWidget(backBtn);
        stack = new QStackedWidget; root->addWidget(bar); root->addWidget(stack, 1);

        stack->addWidget(buildHome());
        stack->addWidget(buildList());
        lessonBox = new QWidget; lessonLay = new QVBoxLayout(lessonBox); lessonLay->setContentsMargins(16, 16, 16, 24);
        lessonScroll = makeScroll(lessonBox); stack->addWidget(lessonScroll);
        stack->addWidget(buildInfo(U("💻"), "Coming Soon",
            "Code Compiler is on the way. Practice Git and code right inside the app. Stay tuned!", "Subscribe for updates"));
        stack->addWidget(buildInfo(U("📘"), "Git & GitHub Handbook",
            "A beginner-friendly guide covering Git, GitHub, branching, merging, undoing mistakes, GitHub Desktop, VS Code and more.\n\nMade by GameDeveloperAnup.",
            "Subscribe @GameDeveloperAnup"));
        buildDrawer();
        showHome();
    }

    // ----- navigation -----
    void go(int page, const QString &title = "", bool back = false) {
        openDrawer(false); curPage = page; stack->setCurrentIndex(page);
        bar->setVisible(page != HOME); barTitle->setText(title); backBtn->setVisible(back);
    }
    void showHome() { go(HOME); }
    void showList() { go(LIST, "Lessons"); }

    void openLesson(int i) {
        while (auto *it = lessonLay->takeAt(0)) { if (it->widget()) it->widget()->deleteLater(); delete it; }
        const Lesson &L = LESSONS[i]; auto *lay = lessonLay;
        lay->addWidget(mk(QString("LESSON %1").arg(i + 1),
            "color:#f04e2f;border:1px solid #f04e2f;border-radius:11px;padding:2px 12px;font-size:12px;font-weight:700;", false), 0, Qt::AlignLeft);
        lay->addWidget(mk(U(L.title), "font-size:26px;font-weight:700;color:#141a2a;"));
        lay->addWidget(mk(U(L.sub), "font-size:14px;color:#667085;"));
        QStringList code;
        auto flush = [&] { if (!code.isEmpty()) { lay->addWidget(new MacWindow(code.join("\n"))); code.clear(); } };
        for (const QString &r : U(L.body).split('\n')) {
            if (r.startsWith("$ ")) { code << r.mid(2); continue; }
            flush();
            if (r.startsWith("# ")) { lay->addSpacing(10);
                lay->addWidget(mk(r.mid(2), "font-size:18px;font-weight:700;color:#141a2a;border-bottom:1px solid #e6e8ec;padding-bottom:4px;")); }
            else if (r.startsWith("- "))
                lay->addWidget(mk("<span style='color:#f04e2f'>&#8226;</span>&nbsp; " + r.mid(2).toHtmlEscaped(), "font-size:15px;color:#667085;"));
            else lay->addWidget(mk(r, "font-size:15px;color:#667085;"));
        }
        flush();
        lay->addSpacing(16);
        auto *nav = new QHBoxLayout;
        if (i > 0) { auto *p = btn(U("‹ Previous"), "transparent", "#f04e2f", "#f04e2f");
            connect(p, &QPushButton::clicked, [this, i] { openLesson(i - 1); }); nav->addWidget(p); }
        if (i < (int)LESSONS.size() - 1) { auto *n = btn(U("Next ›"), "#f04e2f", "white", "#f04e2f");
            connect(n, &QPushButton::clicked, [this, i] { openLesson(i + 1); }); nav->addWidget(n); }
        else { auto *a = btn("All Lessons", "#f04e2f", "white", "#f04e2f");
            connect(a, &QPushButton::clicked, [this] { showList(); }); nav->addWidget(a); }
        lay->addLayout(nav);
        auto *s = btn(U("▶ Subscribe @GameDeveloperAnup"), "transparent", "#f04e2f", "#f04e2f");
        connect(s, &QPushButton::clicked, openYT); lay->addWidget(s); lay->addStretch();
        go(LESSON, QString("Lesson %1").arg(i + 1), true);
        lessonScroll->verticalScrollBar()->setValue(0);
    }

    // ----- pages -----
    QWidget *buildHome() {
        auto *h = new HomeBg; auto *v = new QVBoxLayout(h);
        v->setContentsMargins(24, 24, 24, 24); v->setAlignment(Qt::AlignCenter);
        auto *logo = new QLabel(U("⎇")); logo->setFixedSize(88, 88); logo->setAlignment(Qt::AlignCenter);
        logo->setStyleSheet("background:#f04e2f;color:white;border-radius:24px;font-size:50px;");
        v->addWidget(logo, 0, Qt::AlignHCenter); v->addSpacing(16);
        auto *small = mk(U("THE COMPLETE BEGINNER’S GUIDE"), "color:#c8cdd8;", false); small->setAlignment(Qt::AlignCenter);
        QFont f = small->font(); f.setPixelSize(12); f.setBold(true); f.setLetterSpacing(QFont::AbsoluteSpacing, 2); small->setFont(f);
        v->addWidget(small);
        auto *t = mk("<div align='center' style='font-size:56px;font-weight:900;color:white'>GIT <span style='color:#f04e2f'>&amp;</span><br>GITHUB</div>", "");
        t->setTextFormat(Qt::RichText); v->addWidget(t);
        auto *sub = mk("HANDBOOK", "color:#f04e2f;", false); sub->setAlignment(Qt::AlignCenter);
        QFont f2 = sub->font(); f2.setPixelSize(20); f2.setBold(true); f2.setLetterSpacing(QFont::AbsoluteSpacing, 7); sub->setFont(f2);
        v->addWidget(sub); v->addSpacing(8);
        auto *p = mk("Master version control. Collaborate better. Code with confidence.", "color:#aab1c0;font-size:15px;");
        p->setAlignment(Qt::AlignCenter); p->setMaximumWidth(300); v->addWidget(p, 0, Qt::AlignHCenter); v->addSpacing(16);
        auto *start = btn("Start Learning", "#f04e2f", "white", "#f04e2f"); start->setMinimumWidth(280); start->setMaximumWidth(320);
        connect(start, &QPushButton::clicked, [this] { showList(); }); v->addWidget(start, 0, Qt::AlignHCenter);
        auto *yt = btn(U("▶ Subscribe @GameDeveloperAnup"), "#ff0033", "white", "#ff0033"); yt->setMinimumWidth(280); yt->setMaximumWidth(320);
        connect(yt, &QPushButton::clicked, openYT); v->addWidget(yt, 0, Qt::AlignHCenter); v->addSpacing(12);
        auto *rw = new QWidget; auto *row = new QHBoxLayout(rw); row->setSpacing(10); row->setContentsMargins(0, 0, 0, 0); rw->setMaximumWidth(320);
        QList<QPair<QString, QString>> fs = {{U("📚"), QString::number((int)LESSONS.size()) + " Lessons"}, {U("🖥️"), "Mac-style Code"}, {U("💻"), "Compiler Soon"}};
        for (auto &x : fs) {
            auto *c = new QFrame; c->setObjectName("chip");
            c->setStyleSheet("QFrame#chip{background:rgba(255,255,255,15);border:1px solid rgba(255,255,255,26);border-radius:14px;}");
            auto *cl = new QVBoxLayout(c); cl->setContentsMargins(4, 10, 4, 10);
            auto *e = mk(x.first, "font-size:24px;", false); e->setAlignment(Qt::AlignCenter);
            auto *tx = mk(x.second, "font-size:12px;color:#c8cdd8;", true); tx->setAlignment(Qt::AlignCenter);
            cl->addWidget(e); cl->addWidget(tx); row->addWidget(c);
        }
        v->addWidget(rw, 0, Qt::AlignHCenter); v->addSpacing(14);
        auto *foot = mk(U("Beginner friendly • Practical • By GameDeveloperAnup"), "color:#8b93a5;font-size:12px;"); foot->setAlignment(Qt::AlignCenter);
        v->addWidget(foot);
        return h;
    }
    QWidget *buildList() {
        auto *box = new QWidget; auto *v = new QVBoxLayout(box); v->setContentsMargins(16, 16, 16, 16); v->setSpacing(10);
        for (int i = 0; i < (int)LESSONS.size(); i++) {
            auto *c = new Clickable; c->setObjectName("card");
            c->setStyleSheet("QFrame#card{background:white;border:1px solid #e6e8ec;border-radius:14px;}");
            c->onClick = [this, i] { openLesson(i); };
            auto *h = new QHBoxLayout(c); h->setContentsMargins(14, 14, 14, 14); h->setSpacing(14);
            auto *n = new QLabel(QString::number(i + 1)); n->setFixedSize(34, 34); n->setAlignment(Qt::AlignCenter);
            n->setStyleSheet("background:#f04e2f;color:white;border-radius:17px;font-weight:700;font-size:14px;");
            auto *tv = new QVBoxLayout; tv->setSpacing(0);
            tv->addWidget(mk(U(LESSONS[i].title), "font-size:16px;font-weight:600;color:#141a2a;"));
            tv->addWidget(mk(U(LESSONS[i].sub), "font-size:13px;color:#667085;"));
            h->addWidget(n); h->addLayout(tv, 1); h->addWidget(mk(U("›"), "font-size:24px;color:#667085;", false));
            v->addWidget(c);
        }
        v->addStretch();
        return makeScroll(box);
    }
    QWidget *buildInfo(const QString &emoji, const QString &title, const QString &text, const QString &btnText) {
        auto *w = new QWidget; auto *v = new QVBoxLayout(w); v->setContentsMargins(24, 24, 24, 24); v->setAlignment(Qt::AlignCenter);
        auto *e = mk(emoji, "font-size:56px;", false); e->setAlignment(Qt::AlignCenter);
        auto *t = mk(title, "font-size:26px;font-weight:700;color:#141a2a;"); t->setAlignment(Qt::AlignCenter);
        auto *p = mk(text, "font-size:15px;color:#667085;"); p->setAlignment(Qt::AlignCenter);
        auto *b = btn(U("▶ ") + btnText, "#f04e2f", "white", "#f04e2f"); connect(b, &QPushButton::clicked, openYT);
        v->addWidget(e); v->addWidget(t); v->addWidget(p); v->addSpacing(12); v->addWidget(b);
        return w;
    }

    // ----- drawer (burger menu) -----
    void buildDrawer() {
        dim = new QWidget(this); dim->setAttribute(Qt::WA_StyledBackground);
        dim->setStyleSheet("background:rgba(0,0,0,130);"); dim->installEventFilter(this); dim->hide();
        drawer = new QFrame(this); drawer->setObjectName("drawer");
        drawer->setStyleSheet("QFrame#drawer{background:white;border-right:1px solid #e6e8ec;}");
        auto *v = new QVBoxLayout(drawer); v->setContentsMargins(0, 24, 0, 16); v->setSpacing(0);
        auto *hd = mk("Git & <span style='color:#f04e2f'>GitHub</span>", "font-size:18px;font-weight:700;color:#141a2a;padding:0 20px 14px 20px;", false);
        hd->setTextFormat(Qt::RichText); v->addWidget(hd);
        struct Item { QString text; std::function<void()> fn; };
        QList<Item> items = {
            {U("🏠  Home"), [this] { showHome(); }},
            {U("📚  All Lessons"), [this] { showList(); }},
            {U("💻  Code Compiler    • Soon"), [this] { go(SOON, "Code Compiler"); }},
            {U("ℹ️  About"), [this] { go(ABOUT, "About"); }},
            {U("▶  Subscribe on YouTube"), [] { openYT(); }},
        };
        for (auto &it : items) {
            auto *b = new QPushButton(it.text); b->setFlat(true);
            b->setStyleSheet("QPushButton{text-align:left;padding:14px 20px;font-size:16px;font-weight:600;color:#141a2a;border:0;}"
                             "QPushButton:pressed{background:#e6e8ec;}");
            connect(b, &QPushButton::clicked, [fn = it.fn] { fn(); }); v->addWidget(b);
        }
        v->addStretch(); drawer->setGeometry(-DW, 0, DW, height());
    }
    void openDrawer(bool on) {
        if (!drawer) return; drawerOpen = on; dim->setVisible(on);
        if (on) { dim->raise(); drawer->raise(); }
        auto *a = new QPropertyAnimation(drawer, "pos", this);
        a->setDuration(220); a->setEndValue(QPoint(on ? 0 : -DW, 0)); a->start(QAbstractAnimation::DeleteWhenStopped);
    }

protected:
    void resizeEvent(QResizeEvent *e) override {
        QMainWindow::resizeEvent(e);
        if (dim) dim->setGeometry(rect());
        if (drawer) drawer->setGeometry(drawerOpen ? 0 : -DW, 0, DW, height());
    }
    bool eventFilter(QObject *o, QEvent *e) override {
        if (o == dim && e->type() == QEvent::MouseButtonPress) { openDrawer(false); return true; }
        return QMainWindow::eventFilter(o, e);
    }
    void keyPressEvent(QKeyEvent *e) override {   // Android back button
        if (e->key() == Qt::Key_Back || e->key() == Qt::Key_Escape) {
            if (drawerOpen) openDrawer(false);
            else if (curPage == LESSON) showList();
            else if (curPage != HOME) showHome();
            else QMainWindow::keyPressEvent(e);
        } else QMainWindow::keyPressEvent(e);
    }
};

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    app.setStyle("Fusion");
    QPalette pal; pal.setColor(QPalette::Window, QColor("#fafafa")); pal.setColor(QPalette::WindowText, QColor("#141a2a"));
    app.setPalette(pal);
    Win w;
#ifdef Q_OS_ANDROID
    w.showFullScreen();
#else
    w.resize(400, 780); w.show();
#endif
    return app.exec();
}