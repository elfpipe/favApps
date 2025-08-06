// calculator_nodes.cpp

#include <QtWidgets>
#include <cmath>

struct ButtonDef {
    QString label;
    QPoint gridPos;
};

const QList<ButtonDef> calculatorButtons = {
    {"7", {0, 0}}, {"8", {1, 0}}, {"9", {2, 0}}, {"/", {3, 0}},
    {"4", {0, 1}}, {"5", {1, 1}}, {"6", {2, 1}}, {"*", {3, 1}},
    {"1", {0, 2}}, {"2", {1, 2}}, {"3", {2, 2}}, {"-", {3, 2}},
    {"0", {0, 3}}, {".", {1, 3}}, {"=", {2, 3}}, {"+", {3, 3}},
    {"C", {0, 4}},
};

class NNode : public QGraphicsEllipseItem {
public:
    NNode(const QString &text, QPointF home) : label(text), homePos(home) {
        setRect(-30, -30, 60, 60);
        setBrush(Qt::lightGray);
        setZValue(1);

        QGraphicsTextItem *txt = new QGraphicsTextItem(text, this);
        txt->setDefaultTextColor(Qt::black);
        txt->setFont(QFont("Arial", 14, QFont::Bold));
        QRectF r = txt->boundingRect();
        txt->setPos(-r.width()/2, -r.height()/2);

        setPos(home);
    }

    void updateWithMouse(const QPointF &mouse, qreal radius) {
        QPointF delta = pos() - mouse;
        qreal dx = delta.x();
        qreal dy = delta.y();
        qreal dist2 = dx * dx + dy * dy;

        if (dist2 < radius * radius && dist2 > 0.1) {
            qreal dist = std::sqrt(dist2);
            qreal overlap = radius - dist;
            qreal nx = dx / dist;
            qreal ny = dy / dist;
            QPointF away = QPointF(nx * overlap, ny * overlap);
            setPos(pos() + away * 0.3); // respond with softness
        } else {
            // Glide back to home position
            QPointF back = homePos - pos();
            setPos(pos() + back * 0.1); // smooth return
        }
    }

private:
    QString label;
    QPointF homePos;
};

class Scene : public QGraphicsScene {
public:
    Scene(QObject *parent = nullptr) : QGraphicsScene(parent) {
        setSceneRect(-250, -350, 500, 700);

        int spacing = 70;
        int startX = -105;
        int startY = -100;

        for (const ButtonDef &b : calculatorButtons) {
            QPointF home(startX + b.gridPos.x() * spacing, startY + b.gridPos.y() * spacing);
            NNode *n = new NNode(b.label, home);
            nodes.append(n);
            addItem(n);
        }

        // Display rectangle (decorative)
        QGraphicsRectItem *display = new QGraphicsRectItem(-140, -250, 280, 60);
        display->setBrush(Qt::white);
        display->setPen(QPen(Qt::black, 2));
        addItem(display);
    }

    void updateNodePositions(const QPointF &mouse) {
        for (NNode *n : nodes)
            n->updateWithMouse(mouse, 50.0);
    }

private:
    QList<NNode *> nodes;
};

class View : public QGraphicsView {
public:
    View(Scene *scene) : QGraphicsView(scene), sc(scene) {
        setRenderHint(QPainter::Antialiasing);
        setMouseTracking(true);
        setBackgroundBrush(QColor(220, 220, 220));
    }

protected:
    void mouseMoveEvent(QMouseEvent *event) override {
        QPointF sceneMouse = mapToScene(event->pos());
        sc->updateNodePositions(sceneMouse);
        QGraphicsView::mouseMoveEvent(event);
    }

private:
    Scene *sc;
};

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    Scene *scene = new Scene();
    View *view = new View(scene);
    view->setWindowTitle("Calculator Nodes");
    view->resize(600, 800);

#if 0
    // Placér vinduet på modsatte halvdel af hvor musen er
    QPoint cursorPos = QCursor::pos();
    QRect screenRect = QGuiApplication::primaryScreen()->availableGeometry();

    int halfW = screenRect.width() / 2;
    int halfH = screenRect.height() / 2;

    int winW = 600;
    int winH = 800;

    int x = (cursorPos.x() > halfW) ? halfW - winW : halfW;
    int y = halfH - winH/2;

    view->move(x, y);
#endif
    view->show();

    return app.exec();
}