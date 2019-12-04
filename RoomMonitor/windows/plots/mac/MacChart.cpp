#include "MacChart.h"

MacChart::MacChart(QGraphicsItem *parent, Qt::WindowFlags wFlags) : QChart(parent, wFlags) {
    QBarSet *set0 = new QBarSet("Presenze");
    QHorizontalBarSeries *bs = new QHorizontalBarSeries();
    bs->append(set0);
    this->addSeries(bs);
    this->setTitle("Numero di rilevazioni dei MAC presenti");
    QFont f = this->titleFont();
    f.setBold(true);
    this->setTitleFont(f);
    listMac.clear();
    aY = new QBarCategoryAxis();
    aY->append(listMac);
    this->addAxis(aY, Qt::AlignLeft);
    barSeries->attachAxis(aY);

    aX = new QValueAxis();
    this->addAxis(aX, Qt::AlignBottom);
    barSeries->attachAxis(aX);
    this->legend()->setVisible(false);
}


void MacChart::addSeries(QHorizontalBarSeries *series) {
    this->barSeries = series;
    QChart::addSeries(series);
}

QHorizontalBarSeries *MacChart::getBarSeries() {
    return barSeries;
}

void MacChart::resetView(int maxMac) {
    if (this->isZoomed())
        this->zoomReset();
//    aX->setRange(0, 20);
    if (!listMac.isEmpty())
        aY->setRange(listMac[0], listMac[maxMac < listMac.size() ? maxMac : listMac.size() - 1]);
}

QStringList MacChart::getMacs() {
    return listMac;
}

void MacChart::fillChart(QVector<MacOccurrence> macs) {
    int max = 0;
    this->removeAllSeries();
    std::sort(macs.begin(), macs.end());
    QBarSet *set0 = new QBarSet("Presenze");
    QHorizontalBarSeries *bs = new QHorizontalBarSeries();
    bs->append(set0);
    listMac.clear();
    for (int i = 0; i < macs.size(); i++) {
        *set0 << macs[i].getNumberOfOccurrences();
        if (max < macs[i].getNumberOfOccurrences())
            max = macs[i].getNumberOfOccurrences();
        listMac << macs[i].getMac();
    }
    this->addSeries(bs);

    this->setTitle("Numero di rilevazioni dei MAC presenti");
    aY = new QBarCategoryAxis();
    aY->append(listMac);
    this->addAxis(aY, Qt::AlignLeft);
    barSeries->attachAxis(aY);

    aX = new QValueAxis();
    this->addAxis(aX, Qt::AlignBottom);
    barSeries->attachAxis(aX);
    this->legend()->setVisible(false);
    this->resetView(7);
    aX->setRange(0, max + 5);

}

void MacChart::fillRandomChart(QList<Statistic> macs) {
    int max = 0;
    this->removeAllSeries();
    std::sort(macs.begin(), macs.end());
    QBarSet *set0 = new QBarSet("Percentuale");
    QHorizontalBarSeries *bs = new QHorizontalBarSeries();
    bs->append(set0);
    listMac.clear();
    for (auto &it : macs) {
        *set0 << it.getPerc();
        listMac << it.getMac();
    }
    max = 100;
    this->addSeries(bs);

    this->setTitle("Percentuale somiglianza");
    aY = new QBarCategoryAxis();
    aY->append(listMac);
    this->addAxis(aY, Qt::AlignLeft);
    barSeries->attachAxis(aY);

    aX = new QValueAxis();
    this->addAxis(aX, Qt::AlignBottom);
    barSeries->attachAxis(aX);
    this->legend()->setVisible(false);
    this->resetView(7);
    aX->setRange(0, max + 5);

}
