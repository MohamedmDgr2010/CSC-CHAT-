#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>
#include <string>
#include <vector>
#include "ws.h"
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// تعريف هيكل المستخدم
struct User {
    string id, name;
    User(string i, string n = "") : id(i), name(n) {}
};

// نافذة الدردشة
class ChatWindow : public QWidget {
    Q_OBJECT

public:
    void Show(QString t) {  // Fixed: QString instead of Qstring
        if(t.isEmpty()) return;  // Fixed: isEmpty() instead of empty()
        QLabel* l = new QLabel(t);
        contentLayout->addWidget(l);
    }
    ChatWindow(std::string With, QWidget* parent = nullptr) : QWidget(parent) {
        setupUi(With);
    }

private:
    QVBoxLayout* contentLayout;
    QLineEdit* input;
    std::string With;

    void setupUi(std::string w) {
        With = w;
        // جعل التخطيط الرئيسي لتابع للكلاس نفسه (this)
        QVBoxLayout* mainLayout = new QVBoxLayout(this);

        // --- التخطيط العلوي ---
        QHBoxLayout* topLayout = new QHBoxLayout;
        topLayout->addWidget(new QPushButton(QString::fromStdString(With)));
        mainLayout->addLayout(topLayout);

        // --- إعداد الودجت الأوسط داخل Scroll Area ---
        QWidget* scrollContent = new QWidget;
        contentLayout = new QVBoxLayout(scrollContent);
        scrollContent->setLayout(contentLayout);

        QScrollArea* scrollArea = new QScrollArea;
        scrollArea->setWidgetResizable(true);
        scrollArea->setWidget(scrollContent);
        mainLayout->addWidget(scrollArea, 1);

        // --- التخطيط السفلي ---
        QHBoxLayout* bottomLayout = new QHBoxLayout;
        input = new QLineEdit();
        QPushButton* addButton = new QPushButton("Send");
        bottomLayout->addWidget(input);
        bottomLayout->addWidget(addButton);
        mainLayout->addLayout(bottomLayout);

        // --- الإجراء عند الضغط ---
        connect(addButton, &QPushButton::clicked, this, &ChatWindow::onConditionMet);
    }

private slots:
    void onConditionMet() {
        std::string body = input->text().toStdString();
        if (body.empty()) return;

        QLabel* newLabel = new QLabel(QString::fromStdString(body));
        QString c = "blue";

        newLabel->setStyleSheet("background:" + c + ";max-width:300px;max-height:100px;font-size:35px;");
        contentLayout->addWidget(newLabel);
        ws::send(body, With);
        input->clear();
    }
};
// قائمة المحادثات (مُصححة)
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QPushButton>

class ChatsMenu : public QWidget {
    Q_OBJECT

public:
    ChatsMenu(QWidget* parent = nullptr) : QWidget(parent) {
        menu = new QVBoxLayout(this);
        search = new QLineEdit;
        search->setPlaceholderText("البحث في المحادثات...");
        menu->addWidget(search);

        loadChats();

        connect(search, &QLineEdit::textChanged, this, &ChatsMenu::filterChats);
    }

signals:
    void userSelected(QString user);

private:
    QVBoxLayout* menu;
    QLineEdit* search;
    QVector<QJsonObject> chats;

    void loadChats() {
        QFile file("chats.json");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning("تعذر فتح الملف chats.json");
            return;
        }

        QByteArray data = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isArray()) return;

        QJsonArray arr = doc.array();
        for (const QJsonValue& val : arr) {
            if (val.isObject()) {
                QJsonObject obj = val.toObject();
                chats.append(obj);

                QString name = obj["name"].toString();
                QPushButton* btn = new QPushButton(name);
                menu->addWidget(btn);

                // إرسال الإشارة عند الضغط
                connect(btn, &QPushButton::clicked, this, [=]() {
                    emit userSelected(name);
                });
            }
        }
    }

    void filterChats(const QString& text) {
        // تنفيذ الفلترة حسب النص لاحقًا
    }
};
class MW : public QMainWindow {
    Q_OBJECT

public:
    MW(QWidget* parent = nullptr) : QMainWindow(parent) {
        QWidget* central = new QWidget(this);
        QVBoxLayout* mainLayout = new QVBoxLayout(central);

        QHBoxLayout* header = new QHBoxLayout;
        header->addWidget(new QLabel("Welcome"));  // عنوان أعلى

        QHBoxLayout* app = new QHBoxLayout;

        // القائمة الجانبية
        ChatsMenu* chatsMenu = new ChatsMenu;
        app->addWidget(chatsMenu);

        // شباك المحادثة (يبدأ فارغ)
        chatWindow = nullptr;
        placeholder = new QLabel("No chat selected");
        placeholder->setAlignment(Qt::AlignCenter);
        app->addWidget(placeholder);

        mainLayout->addLayout(header);
        mainLayout->addLayout(app);

        central->setLayout(mainLayout);
        setCentralWidget(central);

        // عند الضغط على مستخدم في القائمة
        connect(chatsMenu, &ChatsMenu::userSelected, this, &MW::openChat);
    }

private slots:
    void openChat(QString user) {
        if (chatWindow != nullptr) {
            placeholder->hide();
            layout()->removeWidget(chatWindow);
            delete chatWindow;
            chatWindow = nullptr;
        }

        chatWindow = new ChatWindow(user.toStdString());
        layout()->addWidget(chatWindow);
    }

private:
    ChatWindow* chatWindow;
    QLabel* placeholder;
};
// تضمين ملف MOC
#include "gui.moc"

// ---- نقطة الدخول للتطبيق ----
int main(int argc, char* argv[]) {
    ws::conn();
    QApplication app(argc, argv);
    
    MW window;
    window.show();
    return app.exec();
}