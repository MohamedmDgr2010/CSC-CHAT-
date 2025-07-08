#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>
#include <QString>
#include <vector>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

class ChatWindow : public QWidget {
    Q_OBJECT

public:
    ChatWindow(QString chatWith, QWidget* parent = nullptr) : QWidget(parent) {
        QVBoxLayout* layout = new QVBoxLayout(this);
        QLabel* label = new QLabel("محادثة مع: " + chatWith);
        label->setStyleSheet("font-size: 20px;");
        layout->addWidget(label);
        layout->addStretch();
    }
};

class ChatsMenu : public QMainWindow {
    Q_OBJECT

public:
    ChatsMenu(QWidget* parent = nullptr) : QMainWindow(parent) {
        // المكون الأساسي
        QWidget* centralWidget = new QWidget(this);
        QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

        // ===== شريط علوي =====
        QWidget* topBar = new QWidget;
        topBar->setStyleSheet("background-color: black;");
        topBar->setFixedHeight(40);
        mainLayout->addWidget(topBar);

        // ===== الوسط: (قائمة + محادثة) =====
        QHBoxLayout* middleLayout = new QHBoxLayout;

        // القائمة الجانبية
        chatListWidget = new QWidget;
        chatListWidget->setStyleSheet("background-color: black;");
        chatListWidget->setFixedWidth(120);
        chatListLayout = new QVBoxLayout(chatListWidget);
        middleLayout->addWidget(chatListWidget);

        // منطقة المحادثة
        chatAreaWidget = new QWidget;
        chatAreaWidget->setStyleSheet("background-color: #888;");
        chatAreaLayout = new QVBoxLayout(chatAreaWidget);
        middleLayout->addWidget(chatAreaWidget);

        // إضافة الوسط للواجهة
        mainLayout->addLayout(middleLayout);
        centralWidget->setLayout(mainLayout);
        setCentralWidget(centralWidget);

        // ===== إنشاء أزرار للمحادثات =====
        cs = {"Ahmad", "Mohamed", "Sara"};
        for (auto c : cs) {
            QString name = QString::fromStdString(c.get<string>());
            QPushButton* btn = new QPushButton(name);
            btn->setStyleSheet("background-color: white; color: black;");
            chatListLayout->addWidget(btn);

            connect(btn, &QPushButton::clicked, this, [=]() {
                openChat(name);
            });
        }
    }

private:
    QWidget* chatListWidget;
    QVBoxLayout* chatListLayout;

    QWidget* chatAreaWidget;
    QVBoxLayout* chatAreaLayout;

    vector<json> cs;

    void openChat(QString name) {
        // حذف المحادثة السابقة
        QLayoutItem* child;
        while ((child = chatAreaLayout->takeAt(0)) != nullptr) {
            delete child->widget();
            delete child;
        }

        // إنشاء محادثة جديدة
        ChatWindow* chat = new ChatWindow(name);
        chatAreaLayout->addWidget(chat);
    }
};

// تضمين MOC
#include "main.moc"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    ChatsMenu window;
    window.resize(800, 600);
    window.setWindowTitle("CSC Chat");
    window.show();
    return app.exec();
}