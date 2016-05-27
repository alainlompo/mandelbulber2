/*
 * material_item_view.cpp
 *
 *  Created on: 20 maj 2016
 *      Author: krzysztof
 */

#include "material_item_view.h"

#include <qpainter.h>

#include "../qt/material_widget.h"
#include "system.hpp"
#include <qpushbutton.h>
#include <qscrollbar.h>

int cMaterialItemView::iconMargin = 10;
int cMaterialItemView::maxNameHeight = 20;

cMaterialItemView::cMaterialItemView(QWidget *parent) :
		QAbstractItemView(parent)
{
	viewHeight = cMaterialWidget::previewHeight + iconMargin * 3 + horizontalScrollBar()->height()
			+ maxNameHeight;
	setMinimumHeight(viewHeight);
	setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	horizontalScrollBar()->setVisible(true);
	setAutoScroll(true);
}

cMaterialItemView::~cMaterialItemView()
{
	// TODO Auto-generated destructor stub
}

QModelIndex cMaterialItemView::indexAt(const QPoint& point) const
{
	int r = (point.x() + horizontalOffset() - iconMargin)
			/ (cMaterialWidget::previewWidth + iconMargin);
	return QModelIndex(model()->index(r, 0, QModelIndex()));
}

QRegion cMaterialItemView::visualRegionForSelection(const QItemSelection& selection) const
{
	return QRegion();
}

void cMaterialItemView::scrollTo(const QModelIndex& index, ScrollHint hint)
{
}

int cMaterialItemView::verticalOffset() const
{
	return verticalScrollBar()->value();
}

int cMaterialItemView::horizontalOffset() const
{
	return horizontalScrollBar()->value();
}

void cMaterialItemView::setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags flags)
{
	viewport()->update();
}

QModelIndex cMaterialItemView::moveCursor(CursorAction cursorAction,
		Qt::KeyboardModifiers modifiers)
{
	return QModelIndex();
}

QRect cMaterialItemView::visualRect(const QModelIndex& index) const
{
	return QRect(	iconMargin + index.row() * (cMaterialWidget::previewWidth + iconMargin),
								iconMargin,
								cMaterialWidget::previewWidth,
								cMaterialWidget::previewHeight + iconMargin + maxNameHeight);
}

bool cMaterialItemView::isIndexHidden(const QModelIndex& index) const
{
	return false;
}

void cMaterialItemView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
		const QVector<int> &roles)
{
	qDebug() << "dataChanged:" << topLeft << bottomRight << roles;
	QString settings = model()->data(topLeft).toString();
	cMaterialWidget *widget = (cMaterialWidget*) indexWidget(topLeft);
	if (widget)
	{
		int materialId = model()->data(topLeft, Qt::UserRole).toInt();
		widget->AssignMaterial(settings, materialId);
	}
	qDebug() << settings;
	qDebug() << indexWidget(topLeft);
	qDebug() << indexWidget(topLeft)->sizeHint();
	qDebug() << visualRect(topLeft);

	viewport()->update();
}

void cMaterialItemView::rowsInserted(const QModelIndex &parent, int start, int end)
{

	qDebug() << "rowsInserted:" << parent << start << end;
	for (int r = start; r <= end; r++)
	{
		cMaterialWidget *widget = new cMaterialWidget(this);
		widget->setAutoFillBackground(true);
		setIndexWidget(model()->index(r, 0, parent), widget);
	}
	QAbstractItemView::rowsInserted(parent, start, end);
	//updateGeometries();

	setCurrentIndex(model()->index(start,0));

	viewport()->update();
}

void cMaterialItemView::resizeEvent(QResizeEvent *event)
{
	QAbstractItemView::resizeEvent(event);
	if (model())
	{
		horizontalScrollBar()->setRange(0,
																		model()->rowCount()
																				* (cMaterialWidget::previewWidth + iconMargin) - width());
	}
	setMinimumHeight(viewHeight);
}

void cMaterialItemView::scrollContentsBy(int dx, int dy)
{
	scrollDirtyRegion(dx, dy);
	viewport()->scroll(dx, dy);
	viewport()->update();
}

void cMaterialItemView::paintEvent(QPaintEvent *event)
{
	QAbstractItemView::paintEvent(event);
	QPainter painter(viewport());
	painter.setRenderHint(QPainter::Antialiasing);

	//paint selection box
	QModelIndex current = currentIndex();
	QRect rectCurrent = visualRect(current);
	rectCurrent.translate(QPoint(-horizontalOffset(), 0));
	rectCurrent.adjust(-3, -3, 3, 3);
	painter.fillRect(rectCurrent, QApplication::palette().highlight());

	//paint labels
	for (int r = 0; r < model()->rowCount(); r++)
	{
		QString name = model()->headerData(r, Qt::Horizontal).toString();
		int x = r * (cMaterialWidget::previewWidth + iconMargin) + iconMargin - horizontalOffset();

		painter.drawText(	QRect(x,
														cMaterialWidget::previewHeight + iconMargin * 2,
														cMaterialWidget::previewWidth,
														maxNameHeight),
											Qt::AlignHCenter | Qt::TextWordWrap,
											name);
	}
}