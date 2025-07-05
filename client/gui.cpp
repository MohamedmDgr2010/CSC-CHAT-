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
#include "ws.h"

strcat User{
  string id,name;
  User(string i,string n=""):id(i),name(n){
    
  }
}
class ChatWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(std::string With,QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUi(With);
    }

private:
    QVBoxLayout* contentLayout;
    QLineEdit* input;
    std::string With;
    void setupUi(std::string w){
        With=w;
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
        input = new QLineEdit();  // حفظه كعضو في الكلاس
        QPushButton* addButton = new QPushButton("Send");
        bottomLayout->addWidget(input);
        bottomLayout->addWidget(addButton);
        mainLayout->addLayout(bottomLayout);

        // --- الإجراء عند الضغط ---
        connect(addButton, &QPushButton::clicked, this, &MainWindow::onConditionMet);

        centralWidget->setLayout(mainLayout);
        setCentralWidget(centralWidget);
    }

private slots:
    void onConditionMet() {
        std::string body = input->text().toStdString();
        if (body.empty()) return;

        QLabel* newLabel = new QLabel(QString::fromStdString(body));
        newLabel->setStyleSheet("background:blue;");
        contentLayout->addWidget(newLabel);
        ws::send(body,With);
        input->clear();
    }
    
};

// ---- نقطة الدخول للتطبيق ----
#include "main.moc"

int main(int argc, char *argv[]) {
    ws::conn();
    QApplication app(argc, argv);
    ChatWindow window("Ahmad");
    window.show();
    return app.exec();
}