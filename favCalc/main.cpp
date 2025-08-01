// mouse_sphere_repulsion.cpp

#include <QtWidgets>
#include <cmath>

class NNode : public QGraphicsEllipseItem {
public:
    NNode(qreal x, qreal y, qreal r = 20.0) {
        setRect(-r, -r, 2*r, 2*r);
        setBrush(Qt::blue);
        setPos(x, y);
        setFlag(QGraphicsItem::ItemIsMovable, false);
        setFlag(QGraphicsItem::ItemIsSelectable, false);
    }

    void avoidMouseSphere(const QPointF &mouse, qreal radius) {
        QPointF delta = pos() - mouse;
        qreal dx = delta.x();
        qreal dy = delta.y();
        qreal dist2 = dx * dx + dy * dy;
        qreal r2 = radius * radius;

        if (dist2 < r2 && dist2 > 0.01) {
            qreal dist = std::sqrt(dist2);
            qreal overlap = radius - dist;
            qreal nx = dx / dist;
            qreal ny = dy / dist;
            QPointF target = pos() + QPointF(nx * overlap, ny * overlap);
            setPos(pos() * 0.8 + target * 0.2); // smooth glide
        }
    }
};

class Scene : public QGraphicsScene {
public:
    Scene(QObject *parent = nullptr) : QGraphicsScene(parent) {
        setSceneRect(-300, -300, 600, 600);

        // Create some nodes
        for (int i = 0; i < 10; ++i) {
            NNode *n = new NNode(QRandomGenerator::global()->bounded(-150, 150), QRandomGenerator::global()->bounded(-150, 150));
            nodes.append(n);
            addItem(n);
        }
    }

    void updateNodesAvoidingMouse(const QPointF &mouse) {
        for (NNode *n : nodes)
            n->avoidMouseSphere(mouse, 80.0); // 80 px radius
    }

private:
    QList<NNode *> nodes;
};

class View : public QGraphicsView {
public:
    View(Scene *scene) : QGraphicsView(scene), sc(scene) {
        setRenderHint(QPainter::Antialiasing);
        setMouseTracking(true);
    }

protected:
    void mouseMoveEvent(QMouseEvent *event) override {
        QPointF sceneMouse = mapToScene(event->pos());
        sc->updateNodesAvoidingMouse(sceneMouse);
        QGraphicsView::mouseMoveEvent(event);
    }

private:
    Scene *sc;
};

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    Scene *scene = new Scene();
    View *view = new View(scene);
    view->setWindowTitle("Mouse Sphere Repulsion");
    view->resize(800, 800);
    view->show();

    return app.exec();
}
