CC = gcc
CFLAGS  := -Wall -O3 -std=c++0x 


OBJS = .o #.o文件与.cpp文件同名
LIB = libgendll.so # 目标文件名 

OPENCV_INC= -I $(OPENCV_INC_ROOT)

INCLUDE_PATH = $(OPENCV_INC)

LIB_PATH = -L $(OPENCV_LIB_ROOT)

# 依赖的lib名称
OPENCV_LIB = -lopencv_objdetect -lopencv_core -lopencv_highgui -lopencv_imgproc

all : $(LIB)

# 2. 生成.o文件 
%.o : %.cpp
    $(CC) $(CFLAGS) -fpic -c $< -o $@ $(INCLUDE_PATH) $(LIB_PATH) $(OPENCV_LIB) 

# 3. 生成动态库文件
$(LIB) : $(OBJS)
    rm -f $@
    g++ -shared -o $@ $(OBJS)
    rm -f $(OBJS)

tags :
    

# 4. 删除中间过程生成的文件 
clean:
    rm -f $(OBJS) $(TARGET) $(LIB)
