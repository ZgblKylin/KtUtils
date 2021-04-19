#include <KtUtils/Json>

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  QString path = QFileDialog::getOpenFileName(
      nullptr, QStringLiteral("Open json file"), QString(),
      QStringLiteral("json file(*.json)"));
  QFileInfo fileInfo(path);
  if (!fileInfo.exists()) return EXIT_FAILURE;

  QFile file(fileInfo.absoluteFilePath());
  if (!file.open(QFile::ReadOnly | QFile::Text)) return EXIT_FAILURE;

  QJsonParseError error;
  QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
  if (error.error != QJsonParseError::NoError) return EXIT_FAILURE;
  file.close();

  QTreeView tree;
  QStandardItemModel* model = new QStandardItemModel;
  tree.setModel(model);
  QList<QStandardItem*> row = KtUtils::Json::toStandardItem(
      doc.isArray() ? QJsonValue(doc.array()) : QJsonValue(doc.object()),
      fileInfo.fileName());
  model->appendRow(row);
  // Object only return 1 item, force model to display 2 columns
  model->setColumnCount(2);
  tree.setRootIndex(model->invisibleRootItem()->index());

  tree.setWindowTitle(fileInfo.absoluteFilePath());
  tree.header()->setSectionResizeMode(QHeaderView::ResizeToContents);
  tree.header()->setResizeContentsPrecision(0);
  tree.setEditTriggers(QAbstractItemView::NoEditTriggers);
  tree.expandAll();
  tree.show();

  QJsonValue json = KtUtils::Json::fromStandardItem(row.at(0));
  doc = QJsonDocument(json.toObject());
  QFile output(fileInfo.absoluteDir().absoluteFilePath(
      fileInfo.completeBaseName() + "_out.json"));
  output.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
  output.write(doc.toJson(QJsonDocument::Indented));
  output.close();
  QDesktopServices::openUrl(output.fileName());

  return app.exec();
}
