#include <windows.h>
#include <windowsx.h>
#include <cstdio>

typedef struct imag {
    int type;//type=1 直线;2 椭圆;3 三角形;0 无图像;
    POINT p[4];//记录直线三角形椭圆左上右下两端点坐标
}image;
image ima;
bool draw_flag_triangle = false;  // 三角形按钮是否按下
bool draw_flag_ellipse = false;  // 椭圆按钮是否按下
bool draw_flag_line = false;  // 线按钮是否按下
bool load_flag = false;  // 重绘按钮是否按下
bool move_flag = false;  // 移动按钮是否按下
bool drawing = false;  // 是否处于绘画状态（三角形）
POINT point[4];  // 画三角形的三个顶点
int is_found = 0;
void init_folder();
// 抽象类
class Shape {
public:
    virtual void draw(HWND hwnd, HPEN hp, POINT* p) = 0;
};

class Move {
public:
    virtual void move(HWND hwnd,int x,int y) = 0;
};

class Save {
public:
    virtual void save(POINT* p) = 0;
};

class Load {
public:
    virtual void load(HWND hwnd) = 0;
};

class Repaint {
public:
    virtual void draw(HWND hwnd) = 0;
};
class EllipseSave : public Save {
public:
    void save(POINT* p) override{
        // 保存椭圆
        FILE* file;
        fopen_s(&file, "ellipse.txt", "a");
        if (file == NULL)
        {
            MessageBox(NULL, TEXT("无法保存椭圆位置信息"), TEXT("错误"), MB_ICONERROR);
            return;
        }
        fprintf(file, "%d %d %d %d\n", p[0].x, p[0].y, p[1].x, p[1].y);
        fclose(file);
        fopen_s(&file, "ellipse_tmp.txt", "a");
        fprintf(file, "%d %d %d %d\n", p[0].x, p[0].y, p[1].x, p[1].y);
        fclose(file);
    }
};

class LineSave : public Save {
public:
    void save(POINT* p) override{
        FILE* file;
        fopen_s(&file, "line.txt", "a");
        if (file == NULL)
        {
            MessageBox(NULL, TEXT("无法保存直线位置信息"), TEXT("错误"), MB_ICONERROR);
            return;
        }
        fprintf(file, "%d %d %d %d\n", p[0].x, p[0].y, p[1].x, p[1].y);
        fclose(file);
        fopen_s(&file, "line_tmp.txt", "a");
        fprintf(file, "%d %d %d %d\n", p[0].x, p[0].y, p[1].x, p[1].y);
        fclose(file);
    }
};

class TriangleSave : public Save {
public:
    void save(POINT* p) override{
        FILE* file;
        fopen_s(&file, "triangle.txt", "a");
        if (file == NULL)
        {
            MessageBox(NULL, TEXT("无法保存椭圆位置信息"), TEXT("错误"), MB_ICONERROR);
            return;
        }
        fprintf(file, "%d %d %d %d\n", p[0].x, p[0].y, p[1].x, p[1].y);
        fclose(file);
        fopen_s(&file, "triangle_tmp.txt", "a");
        fprintf(file, "%d %d %d %d\n", p[0].x, p[0].y, p[1].x, p[1].y);
        fclose(file);
    }
};
// 具体子类
class EllipsePainter : public Shape {
public:
    void ellipsepaint(HWND hwnd, HPEN hp, POINT* p) {
        
        HDC hdc;
        hdc = GetDC(hwnd); //绘画环境为由参数传递的主窗口
        SelectObject(hdc, hp); //将设置好的画笔hp添加到绘画环境里
        Ellipse(hdc, p[0].x, p[0].y, p[1].x, p[1].y);
    }
    void draw(HWND hwnd, HPEN hp, POINT* p)override
    {
        MSG msg;
        int count = 0;
        BOOL bRet;

        while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
        {
            if (count == 0)
            {
                if (msg.message == WM_LBUTTONDOWN)
                {
                    p[0].x = LOWORD(msg.lParam);
                    p[0].y = HIWORD(msg.lParam);
                    p[1].x = LOWORD(msg.lParam);
                    p[1].y = HIWORD(msg.lParam);
                    count++;
                }

            }
            else if (count == 1)
            {
                
                if (msg.message == WM_MOUSEMOVE)
                {
                    ellipsepaint(hwnd, CreatePen(PS_SOLID, 1, RGB(255, 255, 255)), p); //消除上次绘画痕迹的一种方法，利用和背景相同的白色画笔
                    p[1].x = LOWORD(msg.lParam); //终点随着鼠标移动而改变
                    p[1].y = HIWORD(msg.lParam);
                    ellipsepaint(hwnd, CreatePen(PS_SOLID, 1, RGB(0, 0, 0)), p);  //重新绘画
                }
                else if (msg.message == WM_LBUTTONDOWN)
                {
                    EllipseSave().save(p);
                    break;
                }
            }


        }
    }
};

class LinePainter : public Shape {
public:
    void linepaint(HWND hwnd, HPEN hp, POINT* p) { //第二个参数为画笔
        HDC hdc;
        hdc = GetDC(hwnd); //绘画环境为由参数传递的主窗口
        SelectObject(hdc, hp); //将设置好的画笔hp添加到绘画环境里
        MoveToEx(hdc, p[0].x, p[0].y, NULL);
        LineTo(hdc, p[1].x, p[1].y);
    }
    void draw(HWND hwnd, HPEN hp, POINT* p)override
    {
        MSG msg;
        int count = 0;
        BOOL bRet;

        while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
        {
            if (count == 0)
            {
                if (msg.message == WM_LBUTTONDOWN)
                {
                    p[0].x = LOWORD(msg.lParam);
                    p[0].y = HIWORD(msg.lParam);
                    p[1].x = LOWORD(msg.lParam);
                    p[1].y = HIWORD(msg.lParam);
                    count++;
                }

            }
            else if (count == 1)
            {
                
                if (msg.message == WM_MOUSEMOVE)
                {
                    linepaint(hwnd, CreatePen(PS_SOLID, 1, RGB(255, 255, 255)), p); //消除上次绘画痕迹的一种方法，利用和背景相同的白色画笔
                    p[1].x = LOWORD(msg.lParam); //终点随着鼠标移动而改变
                    p[1].y = HIWORD(msg.lParam);
                    linepaint(hwnd, CreatePen(PS_SOLID, 1, RGB(0, 0, 0)), p);  //重新绘画
                }
                else if (msg.message == WM_LBUTTONDOWN)
                {
                    LineSave().save(p);
                    break;
                }
            }


        }
    }
};

class TrianglePainter : public Shape {
public:
    void trianglepaint(HWND hwnd, HPEN hp, POINT* p) {  // 第二个参数为画笔
        HDC hdc;
        int tmp;
        hdc = GetDC(hwnd);  // 绘画环境为由参数传递的主窗口
        SelectObject(hdc, hp);  // 将设置好的画笔hp添加到绘画环境里
        p[2].x = (p[0].x + p[1].x) / 2;
        p[2].y = p[0].y;
        tmp = p[0].y;
        p[0].y = p[1].y;
        Polygon(hdc, p, 3);
        p[0].y = tmp;

        ReleaseDC(hwnd, hdc);  // 释放绘画环境
    }
    void draw(HWND hwnd, HPEN hp, POINT* p)override
    {
        MSG msg;
        int count = 0;
        BOOL bRet;

        while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
        {
            if (count == 0)
            {
                if (msg.message == WM_LBUTTONDOWN)
                {
                    p[0].x = LOWORD(msg.lParam);
                    p[0].y = HIWORD(msg.lParam);
                    p[1].x = LOWORD(msg.lParam);
                    p[1].y = HIWORD(msg.lParam);
                    count++;
                }

            }
            else if (count == 1)
            {
                if (msg.message == WM_MOUSEMOVE)
                {
                    trianglepaint(hwnd, CreatePen(PS_SOLID, 1, RGB(255, 255, 255)), p); //消除上次绘画痕迹的一种方法，利用和背景相同的白色画笔
                    p[1].x = LOWORD(msg.lParam); //终点随着鼠标移动而改变
                    p[1].y = HIWORD(msg.lParam);
                    trianglepaint(hwnd, CreatePen(PS_SOLID, 1, RGB(0, 0, 0)), p);  //重新绘画
                }
                else if (msg.message == WM_LBUTTONDOWN)
                {
                    TriangleSave().save(p);
                    return;
                }

            }


        }
    }
};

class EllipseRepaint : public Repaint {
public:
    void draw(HWND hwnd)override{
        FILE* file;
        int x1, y1, x2, y2;//负责接收txt中的坐标的变量
        fopen_s(&file, "ellipse_tmp.txt", "r");
        if (file == NULL)
        {
            MessageBox(NULL, TEXT("无法读取直线位置信息"), TEXT("错误"), MB_ICONERROR);
        }
        else {
            while (fscanf_s(file, "%d %d %d %d", &x1, &y1, &x2, &y2) == 4)//逐个读取椭圆的位置信息
            {
                //逐个赋值给point变量
                point[0].x = x1;
                point[0].y = y1;
                point[1].x = x2;
                point[1].y = y2;
                EllipsePainter().ellipsepaint(hwnd, CreatePen(PS_SOLID, 1, RGB(0, 0, 0)), point);
            }
        }
        fclose(file);
    }
};

class LineRepaint : public Repaint {
public:
    void draw(HWND hwnd)override {
        FILE* file;
        int x1, y1, x2, y2;//负责接收txt中的坐标的变量
        fopen_s(&file, "line_tmp.txt", "r");
        if (file == NULL)
        {
            MessageBox(NULL, TEXT("无法读取直线位置信息"), TEXT("错误"), MB_ICONERROR);
        }
        else {
            while (fscanf_s(file, "%d %d %d %d", &x1, &y1, &x2, &y2) == 4)//逐个读取直线的位置信息
            {
                //逐个赋值给point变量
                point[0].x = x1;
                point[0].y = y1;
                point[1].x = x2;
                point[1].y = y2;
                LinePainter().linepaint(hwnd, CreatePen(PS_SOLID, 1, RGB(0, 0, 0)), point);
            }
        }
        fclose(file);
    }
};

class TriangleRepaint : public Repaint {
public:
    void draw(HWND hwnd)override {
        FILE* file;
        int x1, y1, x2, y2;//负责接收txt中的坐标的变量
        fopen_s(&file, "triangle_tmp.txt", "r");
        if (file == NULL)
        {
            MessageBox(NULL, TEXT("无法读取直线位置信息"), TEXT("错误"), MB_ICONERROR);
        }
        else {
            while (fscanf_s(file, "%d %d %d %d", &x1, &y1, &x2, &y2) == 4)//逐个读取三角形的位置信息
            {
                //逐个赋值给point变量
                point[0].x = x1;
                point[0].y = y1;
                point[1].x = x2;
                point[1].y = y2;
                TrianglePainter().trianglepaint(hwnd, CreatePen(PS_SOLID, 1, RGB(0, 0, 0)), point);
            }
        }
        fclose(file);
    }
};

class EllipseMove : public Move {
public:
    void move(HWND hwnd,int x,int y) override {
        MSG msg;
        int count = 0;
        BOOL bRet;
        FILE* file;
        int x1, y1, x2, y2;//负责接收txt中的坐标的变量
        int dx, dy;//负责记录鼠标移动的差值
        fopen_s(&file, "ellipse_tmp.txt", "r");
        if (file == NULL)
        {
            MessageBox(NULL, TEXT("无法读取椭圆位置信息"), TEXT("错误"), MB_ICONERROR);
        }
        else {
                while (fscanf_s(file, "%d %d %d %d", &x1, &y1, &x2, &y2) == 4)//逐个读取椭圆的位置信息
                {
                    //逐个赋值给point变量
                    point[0].x = x1;
                    point[0].y = y1;
                    point[1].x = x2;
                    point[1].y = y2;
                    if ((x >= x1 && x <= x2) || (x <= x1 && x >= x2))
                    {
                        if ((y >= y1 && y <= y2) || (y <= y1 && y >= y2))
                        {
                            ima.type = 2;
                            ima.p[0].x = x1;
                            ima.p[0].y = y1;
                            ima.p[1].x = x2;
                            ima.p[1].y = y2;
                            is_found = 1;
                            break;
                        }
                    }
                }
                if(!is_found)   return;//如果没有找到一个符合的椭圆 那么直接终止
            while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
            {
                if (msg.message == WM_MOUSEMOVE)
                    {
                        EllipsePainter().ellipsepaint(hwnd, CreatePen(PS_SOLID, 1, RGB(255, 255, 255)), ima.p);  // 消除上次绘画痕迹的一种方法，利用和背景相同的白色画笔
                        point[1].x = LOWORD(msg.lParam);  // 终点随着鼠标移动而改变
                        point[1].y = HIWORD(msg.lParam);
                        dx = point[1].x - point[0].x;
                        dy = point[1].y - point[0].y;
                        point[0].x = point[1].x;
                        point[0].y = point[1].y;
                        ima.p[0].x += dx;
                        ima.p[1].x += dx;
                        ima.p[0].y += dy;
                        ima.p[1].y += dy;
                        EllipsePainter().ellipsepaint(hwnd, CreatePen(PS_SOLID, 1, RGB(0, 0, 0)), ima.p);  // 重新绘画
                    }
                else if (msg.message == WM_LBUTTONDOWN)
                {
                    return;
                }
            }

        }
        fclose(file);
    }
};

class LineMove : public Move {
public:
    void move(HWND hwnd,int x, int y)override {
        MSG msg;
        int count = 0;
        BOOL bRet;
        FILE* file;
        int x1, y1, x2, y2;//负责接收txt中的坐标的变量
        int dx, dy;//负责记录鼠标移动的差值
        float k;//斜率
        int y0;
        fopen_s(&file, "line_tmp.txt", "r");
        if (file == NULL)
        {
            MessageBox(NULL, TEXT("无法读取直线位置信息"), TEXT("错误"), MB_ICONERROR);
        }
        else {
            while (fscanf_s(file, "%d %d %d %d", &x1, &y1, &x2, &y2) == 4)//逐个读取直线的位置信息
            {
                //逐个赋值给point变量
                point[0].x = x1;
                point[0].y = y1;
                point[1].x = x2;
                point[1].y = y2;
                if ((x >= x1 && x <= x2) || (x <= x1 && x >= x2))
                {
                    k = float(y2 - y1) / float(x2 - x1);
                    y0 = int(y1 + k * (x - x1));
                    if (y - y0 <= 4 && y - y0 >= -4)
                    {
                        ima.type = 1;
                        ima.p[0].x = x1;
                        ima.p[0].y = y1;
                        ima.p[1].x = x2;
                        ima.p[1].y = y2;
                        is_found = 1;
                        break;
                        // 找到了符合的直线 跳出循环 开始执行移动直线的部分
                    }
                }
            }
            if (!is_found)   return;//如果没有找到一个符合的直线 那么直接终止
            while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
            {
                if (msg.message == WM_MOUSEMOVE)
                {
                    LinePainter().linepaint(hwnd, CreatePen(PS_SOLID, 1, RGB(255, 255, 255)), ima.p);  // 消除上次绘画痕迹的一种方法，利用和背景相同的白色画笔
                    point[1].x = LOWORD(msg.lParam);  // 终点随着鼠标移动而改变
                    point[1].y = HIWORD(msg.lParam);
                    dx = point[1].x - point[0].x;
                    dy = point[1].y - point[0].y;
                    point[0].x = point[1].x;
                    point[0].y = point[1].y;
                    ima.p[0].x += dx;
                    ima.p[1].x += dx;
                    ima.p[0].y += dy;
                    ima.p[1].y += dy;
                    LinePainter().linepaint(hwnd, CreatePen(PS_SOLID, 1, RGB(0, 0, 0)), ima.p);  // 重新绘画
                }
                else if (msg.message == WM_LBUTTONDOWN)
                {
                    return;
                }
            }
        }
        fclose(file);
    }
};

class TriangleMove : public Move {
public:
    void move(HWND hwnd,int x,int y) override {
        MSG msg;
        int count = 0;
        BOOL bRet;
        FILE* file;
        int x1, y1, x2, y2;//负责接收txt中的坐标的变量
        int dx, dy;//负责记录鼠标移动的差值
        fopen_s(&file, "triangle_tmp.txt", "r");
        if (file == NULL)
        {
            MessageBox(NULL, TEXT("无法读取三角形位置信息"), TEXT("错误"), MB_ICONERROR);
        }
        else {
            while (fscanf_s(file, "%d %d %d %d", &x1, &y1, &x2, &y2) == 4)//逐个读取三角形的位置信息
            {
                //逐个赋值给point变量
                point[0].x = x1;
                point[0].y = y1;
                point[1].x = x2;
                point[1].y = y2;
                if ((x >= x1 && x <= x2) || (x <= x1 && x >= x2))
                {
                    if ((y >= y1 && y <= y2) || (y <= y1 && y >= y2))
                    {
                        ima.type = 3;
                        ima.p[0].x = x1;
                        ima.p[0].y = y1;
                        ima.p[1].x = x2;
                        ima.p[1].y = y2;
                        is_found = 1;
                        break;
                    }
                }
            }
            if (!is_found)   return;//如果没有找到一个符合的三角形 那么直接终止
            while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
            {
                if (msg.message == WM_MOUSEMOVE)
                {
                    TrianglePainter().trianglepaint(hwnd, CreatePen(PS_SOLID, 1, RGB(255, 255, 255)), ima.p);  // 消除上次绘画痕迹的一种方法，利用和背景相同的白色画笔
                    point[1].x = LOWORD(msg.lParam);  // 终点随着鼠标移动而改变
                    point[1].y = HIWORD(msg.lParam);
                    dx = point[1].x - point[0].x;
                    dy = point[1].y - point[0].y;
                    point[0].x = point[1].x;
                    point[0].y = point[1].y;
                    ima.p[0].x += dx;
                    ima.p[1].x += dx;
                    ima.p[0].y += dy;
                    ima.p[1].y += dy;
                    TrianglePainter().trianglepaint(hwnd, CreatePen(PS_SOLID, 1, RGB(0, 0, 0)), ima.p);  // 重新绘画
                }

                else if (msg.message == WM_LBUTTONDOWN)
                {
                    return;
                }
            }
        }
        fclose(file);
    }
};

class EllipseLoad : public Load {
public:
    void load(HWND hwnd)override {
        FILE* file, *file2;
        fopen_s(&file, "ellipse.txt", "r");
        fopen_s(&file2, "ellipse_tmp.txt", "a");
        int x1, y1, x2, y2;//负责接收txt中的坐标的变量
        if (file == NULL)
        {
            MessageBox(NULL, TEXT("无法读取椭圆位置信息"), TEXT("错误"), MB_ICONERROR);
        }
        else {

            while (fscanf_s(file, "%d %d %d %d", &x1, &y1, &x2, &y2) == 4)//逐个读取椭圆的位置信息
            {
                //逐个赋值给point变量
                point[0].x = x1;
                point[0].y = y1;
                point[1].x = x2;
                point[1].y = y2;
                EllipsePainter().ellipsepaint(hwnd, CreatePen(PS_SOLID, 1, RGB(0, 0, 0)), point);
                fprintf(file2, "%d %d %d %d\n", point[0].x, point[0].y, point[1].x, point[1].y);
            } 
        }
        fclose(file);
        fclose(file2);
    }
};

class LineLoad : public Load {
public:
    void load(HWND hwnd) override {
        FILE* file,*file2;
        int x1, y1, x2, y2;//负责接收txt中的坐标的变量
        fopen_s(&file, "line.txt", "r");
        fopen_s(&file2, "line_tmp.txt", "a");
        if (file == NULL)
        {
            MessageBox(NULL, TEXT("无法读取直线位置信息"), TEXT("错误"), MB_ICONERROR);
        }
        else {
            while (fscanf_s(file, "%d %d %d %d", &x1, &y1, &x2, &y2) == 4)//逐个读取直线的位置信息
            {
                //逐个赋值给point变量
                point[0].x = x1;
                point[0].y = y1;
                point[1].x = x2;
                point[1].y = y2;
                LinePainter().linepaint(hwnd, CreatePen(PS_SOLID, 1, RGB(0, 0, 0)), point);
                fprintf(file2, "%d %d %d %d\n", point[0].x, point[0].y, point[1].x, point[1].y);
            }
        }
        fclose(file);
        fclose(file2);
    }
};

class TriangleLoad : public Load {
public:
    void load(HWND hwnd) override {
        // 加载三角形
        FILE* file,*file2;
        int x1, y1, x2, y2;//负责接收txt中的坐标的变量
        fopen_s(&file, "triangle.txt", "r");
        fopen_s(&file2, "triangle_tmp.txt", "a");
        if (file == NULL)
        {
            MessageBox(NULL, TEXT("无法读取三角形位置信息"), TEXT("错误"), MB_ICONERROR);
        }
        else {
            while (fscanf_s(file, "%d %d %d %d", &x1, &y1, &x2, &y2) == 4)//逐个读取三角形的位置信息
            {
                //逐个赋值给point变量
                point[0].x = x1;
                point[0].y = y1;
                point[1].x = x2;
                point[1].y = y2;
                TrianglePainter().trianglepaint(hwnd, CreatePen(PS_SOLID, 1, RGB(0, 0, 0)), point);
                fprintf(file2, "%d %d %d %d\n", point[0].x, point[0].y, point[1].x, point[1].y);
            }
        }
        fclose(file);
        fclose(file2);
    }
};

// 抽象工厂类
class AbstractFactory {
public:
    virtual Shape* createShape() = 0;
    virtual Move* createMove() = 0;
    virtual Save* createSave() = 0;
    virtual Load* createLoad() = 0;
    virtual Repaint* repaint() = 0;
};

// 具体工厂类
class EllipseFactory : public AbstractFactory {
public:
    Shape* createShape() {
        return new EllipsePainter();
    }

    Move* createMove() {
        return new EllipseMove();
    }

    Save* createSave() {
        return new EllipseSave();
    }

    Load* createLoad() {
        return new EllipseLoad();
    }

    Repaint* repaint() {
        return new EllipseRepaint();
    }
};

class LineFactory : public AbstractFactory {
public:
    Shape* createShape() {
        return new LinePainter();
    }

    Move* createMove() {
        return new LineMove();
    }

    Save* createSave() {
        return new LineSave();
    }

    Load* createLoad() {
        return new LineLoad();
    }
    
    Repaint* repaint() {
        return new LineRepaint();
    }
};

class TriangleFactory : public AbstractFactory {
public:
    Shape* createShape() {
        return new TrianglePainter();
    }

    Move* createMove() {
        return new TriangleMove();
    }

    Save* createSave() {
        return new TriangleSave();
    }

    Load* createLoad() {
        return new TriangleLoad();
    }
    
    Repaint* repaint() {
        return new TriangleRepaint();
    }
};

AbstractFactory* trianglefactory = new TriangleFactory();
AbstractFactory* ellipsefactory = new EllipseFactory();
AbstractFactory* linefactory = new LineFactory();

void move(HWND hwnd)
{
    MSG msg;
    int count = 0;
    BOOL bRet;
    int x, y;
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (msg.message == WM_LBUTTONDOWN)
        {
            x = LOWORD(msg.lParam);  // 终点随着鼠标移动而改变
            y = HIWORD(msg.lParam);
            break;
        }

    }
    
    EllipseMove().move(hwnd,x,y);
    if (!is_found)
    {
        TriangleMove().move(hwnd,x,y);
        if (!is_found)
        {
            LineMove().move(hwnd,x,y);
        }
    }
    is_found = 0;
}
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    init_folder();//初始化文件夹 把用于储存重绘位置信息的文件夹重置
    static TCHAR szAppName[] = TEXT("MyWindows");
    HWND hwnd;  // 窗口句柄，类似指针（可以看作窗口实例）
    MSG msg;  // 消息是一个结构
    WNDCLASS wndclass;  // 注册窗口类

    wndclass.style = CS_HREDRAW | CS_VREDRAW;  // 窗口类型     改变窗口宽，高时，动态改变窗口
    wndclass.lpfnWndProc = WndProc;  // 指出对应的消息处理函数地址
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;  // 应用程序的实例句柄
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);  // 窗口的图标
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);  // 窗口的光标
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);  // 背景，白色
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;  // 窗口类名字

    if (!RegisterClass(&wndclass))  // 注册窗口类且成功
    {
        MessageBox(NULL, TEXT("窗口类名字"), szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow(szAppName,  // 窗口实例化：wndclass->hwnd
        TEXT("画板"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    ShowWindow(hwnd, iCmdShow);  // 显示窗口
    UpdateWindow(hwnd);  // 更新窗口

    while (GetMessage(&msg, NULL, 0, 0))  // 持续接受消息
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);  // 通过windows调用 callback WndProc，把接收到的消息传给回调函数（WndProc）让其处理
    }

    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)  // 消息循环函数,处理消息
{
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;

    static HWND button;  // 窗口三角形（按钮）的句柄
    static HWND button1;  // 窗口椭圆（按钮）的句柄
    static HWND button2;  // 窗口画线（按钮）的句柄
    static HWND button3;  // 窗口读取（按钮）的句柄
    static HWND button4;  // 窗口重绘（按钮）的句柄
    static HWND button5;  // 窗口移动（按钮）的句柄

    switch (message)  // 判断消息种类
    {
    case WM_CREATE:  // 应用程序绘制
        // 绘制一个按钮
        button2 = CreateWindow(TEXT("Button"), TEXT("画线"),  // 窗口类型为按钮,名称为画线
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // 基本设置
            50, 100, 200, 50,  // 坐标为50,160,长度为200，高度为50
            hwnd, (HMENU)1,
            ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        button1 = CreateWindow(TEXT("Button"), TEXT("画椭圆"),  // 窗口类型为按钮,名称为画椭圆
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // 基本设置
            50, 160, 200, 50,  // 坐标为50,160,长度为200，高度为50
            hwnd, (HMENU)1,
            ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        button = CreateWindow(TEXT("Button"), TEXT("画三角形"),  // 窗口类型为按钮,名称为画三角形
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // 基本设置
            50, 220, 200, 50,  // 坐标为50,100,长度为200，高度为50
            hwnd, (HMENU)1,
            ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        button3 = CreateWindow(TEXT("Button"), TEXT("读取"),  // 窗口类型为按钮,名称为读取
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // 基本设置
            50, 280, 200, 50,  // 坐标为50,160,长度为200，高度为50
            hwnd, (HMENU)1,
            ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        button4 = CreateWindow(TEXT("Button"), TEXT("重绘"),  // 窗口类型为按钮,名称为重绘
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // 基本设置
            50, 340, 200, 50,  // 坐标为50,160,长度为200，高度为50
            hwnd, (HMENU)1,
            ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        button5 = CreateWindow(TEXT("Button"), TEXT("移动"),  // 窗口类型为按钮,名称为移动
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // 基本设置
            50, 400, 200, 50,  // 坐标为50,160,长度为200，高度为50
            hwnd, (HMENU)1,
            ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        return 0;

    case WM_COMMAND:  // 选择窗口菜单项或某个控件发送一条消息给它的父窗口或按下一个快捷键时产生此消息
        if ((HWND)lParam == button2) {  // 画线按钮按下
            linefactory->createShape()->draw(hwnd, CreatePen(PS_SOLID, 1, RGB(255, 255, 255)), point);
        }
        else if ((HWND)lParam == button1) {  // 椭圆按钮按下
            ellipsefactory->createShape()->draw(hwnd, CreatePen(PS_SOLID, 1, RGB(255, 255, 255)), point);
        }
        else if ((HWND)lParam == button) {  // 三角形按钮按下
            trianglefactory->createShape()->draw(hwnd, CreatePen(PS_SOLID, 1, RGB(255, 255, 255)), point);
        }
        else if ((HWND)lParam == button3) {  // 读取按钮按下
            trianglefactory->createLoad()->load(hwnd);
            ellipsefactory->createLoad()->load(hwnd);
            linefactory->createLoad()->load(hwnd);
        }
        else if ((HWND)lParam == button4) {  // 重绘按钮按下
            trianglefactory->repaint()->draw(hwnd);
            linefactory->repaint()->draw(hwnd);
            ellipsefactory->repaint()->draw(hwnd);
        }
        else if ((HWND)lParam == button5) {  // 移动按钮按下
            move(hwnd);
        }
        return 0;

    /*case WM_LBUTTONDOWN:  // 鼠标左键按下消息
        if (draw_flag_triangle || draw_flag_ellipse || draw_flag_line) {  // 若按过按钮
            if (!drawing) {  // 未处于绘画状态下，将drawing设为true进入绘画状态
                point[0].x = GET_X_LPARAM(lParam);
                point[0].y = GET_Y_LPARAM(lParam);
                point[1].x = GET_X_LPARAM(lParam);
                point[1].y = GET_Y_LPARAM(lParam);
                drawing = true;
            }
            else {  // 处于绘画状态下再次按下左键，则结束绘画，复原状态
                saveLocation(draw_flag_triangle, draw_flag_ellipse, draw_flag_line, point);
                drawing = false;
                draw_flag_triangle = false;
                draw_flag_ellipse = false;
                draw_flag_line = false;
                move_flag = false;
                init_p(point);
                init_p(ima.p);
            }
        }
        else if (move_flag) {  // 移动图形状态下，记录起点坐标
            if (!drawing) {  // 未处于绘画状态下，将drawing设为true进入绘画状态
                point[0].x = GET_X_LPARAM(lParam);
                point[0].y = GET_Y_LPARAM(lParam);
                point[1].x = GET_X_LPARAM(lParam);
                point[1].y = GET_Y_LPARAM(lParam);
                ima = is_in_image(point[0].x, point[0].y, hwnd);
                drawing = true;
            }
            else {  // 处于绘画状态下再次按下左键，则结束绘画，复原状态
                drawing = false;
                draw_flag_triangle = false;
                draw_flag_ellipse = false;
                draw_flag_line = false;
                move_flag = false;
                init_p(point);
                init_p(ima.p);
            }
        }
        return 0;

    case WM_MOUSEMOVE:  // 鼠标移动消息

        if (drawing) {  // 处于绘画下发生鼠标移动
            if (draw_flag_line)
            {
                draw_line(hwnd, CreatePen(PS_SOLID, 1, RGB(255, 255, 255)), point);  // 消除上次绘画痕迹的一种方法，利用和背景相同的白色画笔
                point[1].x = GET_X_LPARAM(lParam);  // 终点随着鼠标移动而改变
                point[1].y = GET_Y_LPARAM(lParam);
                draw_line(hwnd, CreatePen(PS_SOLID, 1, RGB(0, 0, 0)), point);  // 重新绘画
            }
            else if (draw_flag_ellipse)
            {
                draw_ellipse(hwnd, CreatePen(PS_SOLID, 1, RGB(255, 255, 255)), point); //消除上次绘画痕迹的一种方法，利用和背景相同的白色画笔
                point[1].x = GET_X_LPARAM(lParam); //终点随着鼠标移动而改变
                point[1].y = GET_Y_LPARAM(lParam);
                draw_ellipse(hwnd, CreatePen(PS_SOLID, 1, RGB(0, 0, 0)), point);  //重新绘画
            }
            else if (draw_flag_triangle) {
                draw_triangle(hwnd, CreatePen(PS_SOLID, 1, RGB(255, 255, 255)), point);  // 消除上次绘画痕迹的一种方法，利用和背景相同的白色画笔
                point[1].x = GET_X_LPARAM(lParam);  // 终点随着鼠标移动而改变
                point[1].y = GET_Y_LPARAM(lParam);
                draw_triangle(hwnd, CreatePen(PS_SOLID, 1, RGB(0, 0, 0)), point);  // 重新绘画
            }
            else if (move_flag)
            {
                //if (ima.type == 0) break;//无图像
                if (ima.type == 1)//直线
                {
                    draw_line(hwnd, CreatePen(PS_SOLID, 1, RGB(255, 255, 255)), ima.p);  // 消除上次绘画痕迹的一种方法，利用和背景相同的白色画笔
                    point[1].x = GET_X_LPARAM(lParam);  // 终点随着鼠标移动而改变
                    point[1].y = GET_Y_LPARAM(lParam);
                    dx = point[1].x - point[0].x;
                    dy = point[1].y - point[0].y;
                    point[0].x = point[1].x;
                    point[0].y = point[1].y;
                    ima.p[0].x += dx;
                    ima.p[1].x += dx;
                    ima.p[0].y += dy;
                    ima.p[1].y += dy;
                    draw_line(hwnd, CreatePen(PS_SOLID, 1, RGB(0, 0, 0)), ima.p);  // 重新绘画
                }
                else if (ima.type == 2)//椭圆
                {
                    draw_ellipse(hwnd, CreatePen(PS_SOLID, 1, RGB(255, 255, 255)), ima.p); //消除上次绘画痕迹的一种方法，利用和背景相同的白色画笔
                    point[1].x = GET_X_LPARAM(lParam);  // 终点随着鼠标移动而改变
                    point[1].y = GET_Y_LPARAM(lParam);
                    dx = point[1].x - point[0].x;
                    dy = point[1].y - point[0].y;
                    point[0].x = point[1].x;
                    point[0].y = point[1].y;
                    ima.p[0].x += dx;
                    ima.p[1].x += dx;
                    ima.p[0].y += dy;
                    ima.p[1].y += dy;
                    draw_ellipse(hwnd, CreatePen(PS_SOLID, 1, RGB(0, 0, 0)), ima.p);  //重新绘画
                }
                else if (ima.type == 3)//三角形
                {
                    draw_triangle(hwnd, CreatePen(PS_SOLID, 1, RGB(255, 255, 255)), ima.p);  // 消除上次绘画痕迹的一种方法，利用和背景相同的白色画笔
                    point[1].x = GET_X_LPARAM(lParam);  // 终点随着鼠标移动而改变
                    point[1].y = GET_Y_LPARAM(lParam);
                    dx = point[1].x - point[0].x;
                    dy = point[1].y - point[0].y;
                    point[0].x = point[1].x;
                    point[0].y = point[1].y;
                    ima.p[0].x += dx;
                    ima.p[1].x += dx;
                    ima.p[0].y += dy;
                    ima.p[1].y += dy;
                    draw_triangle(hwnd, CreatePen(PS_SOLID, 1, RGB(0, 0, 0)), ima.p);  // 重新绘画
                }
            }

        }

        break;*/

    case WM_PAINT:  // 重绘窗口
        hdc = BeginPaint(hwnd, &ps);
        GetClientRect(hwnd, &rect);
        EndPaint(hwnd, &ps);
        return 0;

    case WM_DESTROY:  // 窗口销毁消息，窗口退出时收到，主要做清理窗口数据的工作，窗口已经被销毁，无法在执行窗口操作
        exit(0);

        // 各种消息处理等等
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}
void init_folder()
{
    FILE* fp;
    fopen_s(&fp, "line_tmp.txt", "w+");
    fclose(fp);
    fopen_s(&fp, "triangle_tmp.txt", "w+");
    fclose(fp);
    fopen_s(&fp, "ellipse_tmp.txt", "w+");
    fclose(fp);
}
// 客户端代码
int main()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    return WinMain(hInstance, NULL, NULL, SW_SHOW);
}
