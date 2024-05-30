#include "sweepmap.h"
#include <QMouseEvent>
#include <QPaintEvent>
#include <QOpenGLFunctions>
#include <QImage>
#include <QPainter>
#include <QRandomGenerator>
#include <QTime>
#include <QtDebug>

SweepMap::SweepMap(QWidget *parent)
    : QOpenGLWidget(parent)
{
    setMinimumSize(400, 400);
    m_GameContinue = false;
    m_row = 10;
    m_col = 15;
    m_mineNum = 20;
    CreateMaps();
}

SweepMap::~SweepMap()
{
    ClearMaps();
    glDeleteTextures(13, m_TexturesID);
}

void SweepMap::StartGame(int row, int col, int num)
{
    ClearMaps();

    m_row = row;
    m_col = col;
    m_mineNum = num;

    if(m_row > 30) { m_row = 30; }
    if(m_row < 5) { m_row = 5; }
    if(m_col > 30) { m_col = 30; }
    if(m_row < 5) { m_row = 5; }

    int min_mine = m_row*m_col / 10;
    int max_mine = m_row*m_col / 5;

    if(m_mineNum < min_mine) { m_mineNum = min_mine; }
    if(m_mineNum > max_mine) { m_mineNum = max_mine; }

    CreateMaps();
    m_DigCount = 0;
    m_GameContinue = true;
}

void SweepMap::initializeGL()
{
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
    glEnable(GL_TEXTURE_2D);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    CreateTextures();
}

void SweepMap::resizeGL(int w, int h)
{
    if(w == 0) { w = 1; }
    if ( h == 0 ) { h = 1; }
    m_width = w;
    m_height = h;
    glViewport( 0, 0, w, h );
}

void SweepMap::paintGL()
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    double t_gridW = m_width * 1.0 / m_col;
    double t_gridH = m_height * 1.0 / m_row;
    for(int i = 0; i < m_row; i++) {
        for(int j = 0; j < m_col; j++) {
            double l = j*t_gridW;
            double r = l + t_gridW;
            double t = i*t_gridH;
            double b = t + t_gridH;

            DrawGrid(l, t, r, b, m_surfacemap[i][j], m_infacemap[i][j]);

        }
    }
}

void SweepMap::mousePressEvent(QMouseEvent *mEvent)
{
    QOpenGLWidget::mousePressEvent(mEvent);
    if(!m_GameContinue) { return; }
    archorPt = CalPos(mEvent->pos());
}

void SweepMap::mouseReleaseEvent(QMouseEvent *mEvent)
{
    QOpenGLWidget::mouseReleaseEvent(mEvent);
    if(!m_GameContinue) { return; }
    if(archorPt != CalPos(mEvent->pos())) { return; }

    int x = archorPt.x();
    int y = archorPt.y();
    if(mEvent->button() == Qt::LeftButton) { //Left
        DigGrid(x, y);
    } else if(mEvent->button() == Qt::RightButton) { //Right
        if(m_surfacemap[y][x] == 1) {
            if(m_infacemap[y][x] == CountNeighborGrids(x, y, false, 2)) {
                DetectNeighborhood(x, y);
            }
        } else {
            if(m_surfacemap[y][x] == 2) {
                m_surfacemap[y][x] = 3;
                m_DigCount--;

            } else {
                m_surfacemap[y][x] = 2;
                m_DigCount++;
            }
            emit MineNumberChAanged(m_DigCount);
        }
    }
    CheckWin();
    update();
}

void SweepMap::ClearMaps()
{
    for(int i = 0; i < m_row; i++) {
        delete[] m_surfacemap[i];
        delete[] m_infacemap[i];
    }

    delete[] m_surfacemap;
    delete[] m_infacemap;
}

bool SweepMap::CreateMaps()
{
    if(m_row == 0) { return false; }
    if(m_col == 0) { return false; }
    m_surfacemap = new int*[m_row];
    m_infacemap = new int*[m_col];
    for(int i = 0; i < m_row; i++) {
        m_surfacemap[i] = new int[m_col];
        memset(m_surfacemap[i], 0, sizeof(int)*m_col);
        m_infacemap[i] = new int[m_col];
        memset(m_infacemap[i], 0, sizeof(int)*m_col);
    }
    for(int i = 0; i < m_row; i++) {
        for(int j = 0; j < m_col; j++) {
        }
    }

    QRandomGenerator generatorR, generatorC;
    int seed = QTime::currentTime().msec();
    generatorR.seed(seed);
    generatorC.seed(seed + 10);
    generatorR.bounded(0, m_row);
    generatorC.bounded(0, m_col);
    int count = 0;
    while (count < m_mineNum) {
        int t_y = generatorR.generate() % m_row;
        int t_x = generatorC.generate() % m_col;
        if(m_infacemap[t_y][t_x] < 0) { continue; }
        m_infacemap[t_y][t_x] = -1;
        count++;
    }

    for(int i = 0; i < m_row; i++) {
        for(int j = 0; j < m_col; j++) {
            if(m_infacemap[i][j] == -1) { continue; }
            m_infacemap[i][j] = CountNeighborGrids(j, i);
        }
    }
}

int SweepMap::CountNeighborGrids(int x, int y, bool inface, int id)
{
    int count = 0;
    for(int i = x - 1; i < x + 2; i++) {
        if(i < 0) { continue; }
        if(i >= m_col) { continue; }
        for(int j = y - 1; j < y + 2; j++) {
            if(j < 0) { continue; }
            if(j >= m_row) { continue; }
            if(inface&&m_infacemap[j][i] == id) { count++; }
            else if(!inface&&m_surfacemap[j][i] == id) {count++;}
        }
    }
    return count;
}

int SweepMap::DigGrid(int x, int y)
{
    if(!m_GameContinue) { return 0; }
    if(x < 0 || y < 0 || x > m_col - 1 || y > m_row - 1) { return 0; }
    if(m_surfacemap[y][x] == 1) { return 0; }
    if(m_surfacemap[y][x] == 2) { return 0; }
    m_surfacemap[y][x] = 1;
    int count = 1;
    if(m_infacemap[y][x] == -1) {
        m_GameContinue = false;
        emit GameOver(false);
    } else if(m_infacemap[y][x] == 0) {

        DetectNeighborhood(x, y);
    }
    return count;

}

int SweepMap::DetectNeighborhood(int x, int y)
{
    int count = 0;
    count += DigGrid(x - 1, y);
    count += DigGrid(x + 1, y);
    count += DigGrid(x, y - 1);
    count += DigGrid(x, y + 1);
    count += DigGrid(x - 1, y - 1);
    count += DigGrid(x + 1, y - 1);
    count += DigGrid(x - 1, y + 1);
    count += DigGrid(x + 1, y + 1);

    return count;
}

void SweepMap::CheckWin()
{
    if(!m_GameContinue) { return; }
    int count = 0;
    for(int i = 0; i < m_row; i++) {
        for(int j = 0; j < m_col; j++) {
            if(m_surfacemap[i][j] != 1) { count++; }
            if(count > m_mineNum) { return; }
        }
    }
    m_GameContinue = false;
    emit GameOver(true);
}

void SweepMap::DrawGrid(double l, double t, double r, double b, int surface, int inface)
{
    double gl_l = CalGLX(l);
    double gl_t = CalGLY(t);
    double gl_r = CalGLX(r);
    double gl_b = CalGLY(b);
    QColor fillColor(150, 150, 150);
    if(surface == 1) {
        fillColor = QColor(233, 233, 233);
        if(inface == -1) { QColor(255, 0, 0); }
    }
    //background

    glColor3f(fillColor.red() * 1.0 / 255, fillColor.green() * 1.0 / 255, fillColor.blue() * 1.0 / 255);
    glLineWidth(0.1);
    glBegin(GL_QUADS);
    glVertex3f(gl_l, gl_t, 0.0);
    glVertex3f(gl_r, gl_t, 0.0);
    glVertex3f(gl_r, gl_b, 0.0);
    glVertex3f(gl_l, gl_b, 0.0);
    glEnd();
    int drawId = -1;
    if(surface == 1) { drawId = inface; }
    else if(surface == 2) { drawId = 11; }
    else if(surface == 3) { drawId = 12; }

    if((!m_GameContinue) && inface == -1) {
        if(surface != 2) {
            drawId = 10;
        }
    }
    DrawTexture(drawId, l, t, r, b);
    if((!m_GameContinue) && surface == 2 && inface > -1) {
        double t_gll = 0.75 * gl_l + 0.25 * gl_r;
        double t_glr = 0.25 * gl_l + 0.75 * gl_r;
        double t_glt = 0.75 * gl_t + 0.25 * gl_b;
        double t_glb = 0.25 * gl_t + 0.75 * gl_b;

        glColor3f(1.0, 0.0, 0.0);
        glLineWidth(1.0);
        glBegin(GL_LINES);
        glVertex3f(t_gll, t_glt, 0.0);
        glVertex3f(t_glr, t_glb, 0.0);
        glVertex3f(t_glr, t_glt, 0.0);
        glVertex3f(t_gll, t_glb, 0.0);
        glEnd();
    }
    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(0.5);
    glBegin(GL_LINE_LOOP);
    glVertex3f(gl_l, gl_t, 0.0);
    glVertex3f(gl_r, gl_t, 0.0);
    glVertex3f(gl_r, gl_b, 0.0);
    glVertex3f(gl_l, gl_b, 0.0);
    glEnd();
}

void SweepMap::DrawTexture(int id, double l, double t, double r, double b)
{
    if(id == 0) { return; }

    double gl_l = CalGLX(l);
    double gl_t = CalGLY(t);
    double gl_r = CalGLX(r);
    double gl_b = CalGLY(b);


    // 绘制纹理

    glEnable(GL_BLEND);
    //glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, m_TexturesID[id]);

//    glBegin(GL_QUADS);
//    glTexCoord2f(0.0f, 1.0f);
//    glVertex3f(gl_l, gl_b, 0.0);
//    glTexCoord2f(1.0f, 1.0f);
//    glVertex3f(gl_r, gl_b, 0.0);
//    glTexCoord2f(1.0f, 0.0f);
//    glVertex3f(gl_r, gl_t, 0.0);
//    glTexCoord2f(0.0f, 0.0f);
//    glVertex3f(gl_l, gl_t, 0.0);
//    glEnd();

    glBegin(GL_TRIANGLES);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(gl_l, gl_b, 0.0);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(gl_r, gl_b, 0.0);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(gl_r, gl_t, 0.0);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(gl_r, gl_t, 0.0);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(gl_l, gl_t, 0.0);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(gl_l, gl_b, 0.0);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_BLEND);
}

void SweepMap::CreateTextures()
{
    glGenTextures(13, m_TexturesID);
    for(int i = 0; i < 10; i++) { //0-9
        QImage img(28, 28, QImage::Format_RGBA8888);
        img.fill(Qt::transparent);

        QPainter painter(&img);
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 10));
        painter.drawText(img.rect(), Qt::AlignCenter, QString::number(i));

        glBindTexture(GL_TEXTURE_2D, m_TexturesID[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

    }
    {
        //10.bomb
        QImage img(28, 28, QImage::Format_RGBA8888);
        img.fill(Qt::transparent);

        QPainter painter(&img);
        painter.setPen(Qt::black);
        painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
        painter.drawEllipse(QPoint(16, 16), 8, 8);
        QVector<QPoint> pts;
        pts << QPoint(10, 8) << QPoint(8, 10) << QPoint(12, 14) << QPoint(14, 12) << QPoint(10, 8);
        painter.drawPolygon(QPolygon(pts));

        glBindTexture(GL_TEXTURE_2D, m_TexturesID[10]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    {
        //11.mound
        QImage img(28, 28, QImage::Format_RGBA8888);
        img.fill(Qt::transparent);

        QPainter painter(&img);
        painter.setPen(Qt::black);
        painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
        QVector<QPoint> pts0, pts1;
        pts0 << QPoint(4, 24) << QPoint(14, 17) << QPoint(24, 24) << QPoint(4, 24);
        pts1 << QPoint(14, 6) << QPoint(20, 8) << QPoint(14, 10) << QPoint(14, 6);
        painter.drawPolygon(QPolygon(pts0));
        painter.drawPolygon(QPolygon(pts1));
        painter.drawRect(QRect(13, 4, 2, 16));

        glBindTexture(GL_TEXTURE_2D, m_TexturesID[11]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    {
        //12.check
        QImage img(28, 28, QImage::Format_RGBA8888);
        img.fill(Qt::transparent);

        QPainter painter(&img);
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 10));
        painter.drawText(img.rect(), Qt::AlignCenter, "?");

        glBindTexture(GL_TEXTURE_2D, m_TexturesID[12]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

QPoint SweepMap::CalPos(QPoint pos)
{
    double gridW = m_width * 1.0 / m_col;
    double gridH = m_height * 1.0 / m_row;

    int x = floor(pos.x() / gridW);
    int y = floor(pos.y() / gridH);

    return QPoint(x, y);
}
