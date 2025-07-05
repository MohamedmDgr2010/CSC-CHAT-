#include "ws.h"

int main() {
    ws::conn();  // يبدأ الاتصال

    // ب77قاء البرنامج شغال حتى نغلقه يدويًا أو حتى يحصل خطأ
    ws::input();
    return 0;
}
