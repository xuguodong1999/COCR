#include "format_dialog.h"
#include "ui_format_dialog.h"

FormatDialog::FormatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormatDialog)
{
    ui->setupUi(this);
}

FormatDialog::~FormatDialog()
{
    delete ui;
}
