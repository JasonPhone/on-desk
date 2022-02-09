#include "memomanager.h"
#include "./ui_memomanager.h"

MemoManager::MemoManager(QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::MemoManager)
{
  ui->setupUi(this);
}

MemoManager::~MemoManager()
{
  delete ui;
}

