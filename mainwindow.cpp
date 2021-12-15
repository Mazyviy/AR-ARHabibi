#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "glwidget.h"
#include <GL/gl.h>
#include <GL/glaux.h>
#include <GL/glu.h>
#include <math.h>
#include <glut.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("AR / AR Хабиби");
    ui->widget->xAxis->setRange(0, 100);//Для оси Ox
    ui->widget->yAxis->setRange(-3, 3);//Для оси Oy
}

MainWindow::~MainWindow()
{
    delete ui;
}
int q1,q2;
double ds1,ds2,ds3;
double n_x[1000][1000], x[1000][1000], ksi[1000][1000];
int valueStr = 70, valueCol=100;

void normirovanie(double m[1000][1000])
{
    double max, min;
    max = m[0][0];
    min = m[0][0];
    for (int i = 0; i < valueStr; i++)
    {
        for (int j = 0; j < valueCol; j++)
        {
            if (m[i][j] < min) min = m[i][j];
            if ((m[i][j]) > max) max = m[i][j];
        }
    }
    for (int i = 0; i < valueStr; i++)
    {
        for (int j = 0; j < valueCol; j++)
        {
            n_x[i][j] = (m[i][j] - min) / (max - min);
        }
    }
}

void Habibi()
{
    double  sigm, ro1, ro2;
    ro1 = ds2;
    ro2 = ds3;
    sigm = ds1;
    double sum, temp;
    srand((unsigned)(time(NULL)));
    for (int i = 0; i < valueStr; i++)
    {
        for (int j = 0; j < valueCol; j++)
        {
            sum = 0;
            for (int k = 0; k < 12; k++)
            {
                temp = rand() % 100;
                sum = sum + temp / 100;
             }
             ksi[i][j] = sum - 6;
         }
     }

    x[0][0] = sigm * ksi[0][0];
    for (int i = 1; i < valueCol; i++)
    {
        x[0][i] = ro2 * x[0][i - 1] + sigm * sqrt(1 - pow(ro2, 2)) * ksi[i][0];
    }
    for (int i = 1; i < valueStr; i++)
    {
        x[i][0] = ro1 * x[i - 1][0] + sigm * sqrt(1 - pow(ro1, 2)) * ksi[0][i];
    }

    for (int i = 1; i < valueStr; i++)
    {
        for (int j = 1; j < valueCol; j++)
        {
            x[i][j] = ro1 * x[i - 1][j] + ro2 * x[i][j - 1] - ro1 * ro2 * x[i - 1][j - 1] + sigm * sqrt((1 - pow(ro1, 2)) * (1 - pow(ro2, 2))) * ksi[i][j];
        }
    }

    normirovanie(x);
}

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent){}

void GLWidget::initializeGL()
{
    glClearColor(0, 0, 0, 1.0);
}

void GLWidget::paintGL()
{
    Habibi();
    double ii = 0.00, jj = 0.00;
    for (int i = 0; i < valueStr; i++)
    {
        for (int j = 0; j < valueCol; j++)
        {
            ii = (double)i / 100;
            jj = (double)j / 100;
            glBegin(GL_QUADS);
            glColor3f(n_x[i][j], n_x[i][j], n_x[i][j]);
            glVertex2f(0.00 + jj, 0.01+ii);
            glColor3f(n_x[i][j], n_x[i][j], n_x[i][j]);
            glVertex2f(0.00 + jj, 0.00+ii);
            glColor3f(n_x[i][j], n_x[i][j], n_x[i][j]);
            glVertex2f(0.01 + jj, 0.00+ii);
            glColor3f(n_x[i][j], n_x[i][j], n_x[i][j]);
            glVertex2f(0.01 + jj, 0.01+ii);
            glEnd();
        }
    }
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h); //Установка оси координат
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 1.0, 0.0, 0.5);
    glMatrixMode(GL_MODELVIEW);
}

void MainWindow::on_pushButton_clicked()
{
    double sum, temp;
    QVector<double> ksi(100);
    QVector<double> x(100), y(100); //Массивы координат точек
    for(int i=0;i<100;i++)
    {
        y[i]=i;
    }

    srand((unsigned)(time(NULL)));
    for (int i = 0; i < 100; i++)
    {
        sum = 0;
        for (int k = 0; k < 12; k++)
        {
            temp = rand() % 100;
            sum = sum + temp / 100;
        }
        ksi[i] = sum - 6;
    }

    double ro = ui->doubleSpinBox->value();
    double sigm = 1;
    x[0] = sigm * ksi[0];
    for (int i = 1; i < 100; i++)
    {
        x[i] = ro * x[i - 1] + sigm * sqrt(1-pow(ro,2)) * ksi[i];
    }

    ui->widget->clearGraphs();
    ui->widget->addGraph();
    ui->widget->graph(0)->setData(y, x);
    ui->widget->replot();
}

void MainWindow::on_pushButton_2_clicked()
{
    ds1 = ui->doubleSpinBox_2->value();
    ds2 = ui->doubleSpinBox_4->value();
    ds3 = ui->doubleSpinBox_3->value();
}
