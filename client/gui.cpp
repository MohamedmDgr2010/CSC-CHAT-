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
class ChatWindow : public QMainWindow {
    Q_OBJECT

public:
    ChatWindow(std::string With, QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUi(With);
    }

private:
    QVBoxLayout* contentLayout;
    QLineEdit* input;
    std::string With;

    void setupUi(std::string w) {
        With = w;
        QWidget* centralWidget = new QWidget(this);
        QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

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

        centralWidget->setLayout(mainLayout);
        setCentralWidget(centralWidget);
    }

private slots:
    void show(Qstring t){
      if(t.emtpy())return;
      QLabel* l=new QLabel(t);
      contentLayout->addWidget(l);
    }
    void onConditionMet() {
        std::string body = input->text().toStdString();
        if (body.empty()) return;

        QLabel* newLabel = new QLabel(QString::fromStdString(body));
        QString c="blue";
        
        newLabel->setStyleSheet("background:"+c+";max-width:300px;max-height:100px;font-size :35px;");
        contentLayout->addWidget(newLabel);
        ws::send(body, With);
        input->clear();
    }
};

// قائمة المحادثات (غير مكتملة حالياً)
class ChatsMenu : public QMainWindow {
public:
    vector<json> cs;
    ChatsMenu() {
      
      
      
      
      topLayout->addWidget(new QLabel("CSC CHAT"));
        
        mainLayout->addWidget(menuW);
        mainLayout->addWidget(chatW);
        for (auto c : cs) {
          menuW->addWidget(new QPushButton(c.get<QString>()));
          connect();


       
        }
        
    }

private:
    void SelectChat(QString&w){
      chatW->addWidget(new ChatWindow(w));
      return;
    }
    QWidget* menuW=new QWidget;
    QWidget* chatW=new QWidget;
    
    QVBoxLayout* menuV = new QVBoxLayout(this);
    QHBoxLayout* topLayout = new QHBoxLayout(menuV);
    
    QHBoxLayout* mainLayout=new QHBoxLayout(menuV);
    
    
    
    
    
    
    
    
    
};

// تضمين ملف MOC
#include "gui.moc"

// ---- نقطة الدخول للتطبيق ----
int main(int argc, char* argv[]) {
    ws::conn();
    QApplication app(argc, argv);
    ChatWindow window("Ahmad");
    window.show();
    return app.exec();
}
