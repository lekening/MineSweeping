#ifndef SWEEPMAP_H
#define SWEEPMAP_H
#include <QOpenGLWidget>
#include <QObject>

class SweepMap : public QOpenGLWidget
{
    Q_OBJECT
public:
    SweepMap(QWidget* parent = nullptr);
    ~SweepMap();

    void StartGame(int row, int col, int num);

    int getRow() {return m_row;}
    int getCol() {return m_col;}
    int getNum() {return m_mineNum;}

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent* mEvent);
    void mouseReleaseEvent(QMouseEvent* mEvent);

private:
    int m_row, m_col;
    int m_mineNum;

    int m_width, m_height;

    int** m_surfacemap;//0-未开 1-挖开 2-埋葬 3-重新确认
    int** m_infacemap;//0-8附近炸弹数 -1炸弹

    bool m_GameContinue;

    GLuint m_TexturesID[13];//0-9,bomb,mound,check,total13

    QPoint archorPt;
    int m_DigCount;
signals:
    void MineNumberChAanged(int n);
    void GameOver(bool win);

private:
    void ClearMaps();
    bool CreateMaps();
    int CountNeighborGrids(int x, int y, bool inface = true, int id = -1);

    int DigGrid(int x, int y);
    int DetectNeighborhood(int x, int y);
    void CheckWin();

    void DrawGrid(double l, double t, double r, double b, int surface, int inface);
    void DrawTexture(int id, double l, double t, double r, double b);
    void CreateTextures();

    QPoint CalPos(QPoint pos);
    double CalGLX(double x) {return x * 2.0 / m_width - 1.0;}
    double CalGLY(double y) {return 1.0 - y * 2.0 / m_height;}


};

#endif // SWEEPMAP_H
