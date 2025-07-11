# إعداد المتغيرات
CXX = g++
CXXFLAGS = -std=c++23 -O0 -g -march=native -w
LDFLAGS = -lencode -lssl -lcrypto -lcurl -lmysqlclient -lixwebsocket -lz
SRC = main.cpp ../shared/encode.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = app

# الهدف الرئيسي
all: $(TARGET)

# ربط البرنامج النهائي
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

# ترجمة كل ملف .cpp إلى .o مع تتبع ملفات الهيدر
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# نظافة
clean:
	rm -f $(OBJ) $(TARGET)
