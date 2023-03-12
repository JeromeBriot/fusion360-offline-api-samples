#include <Core/Utils.h>
#include <Core/Application/Application.h>
#include <Core/Application/Color.h>
#include <Core/Application/Document.h>
#include <Core/Application/ObjectCollection.h>
#include <Core/Application/Product.h>
#include <Core/Application/Products.h>
#include <Core/Application/ValueInput.h>
#include <Core/Geometry/Arc3D.h>
#include <Core/Geometry/Circle3D.h>
#include <Core/Geometry/BoundingBox3D.h>
#include <Core/Geometry/Ellipse3D.h>
#include <Core/Geometry/EllipticalArc3D.h>
#include <Core/Geometry/Line3D.h>
#include <Core/Geometry/NurbsCurve3D.h>
#include <Core/Geometry/Point2D.h>
#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/Vector2D.h>
#include <Core/Geometry/Vector3D.h>
#include <Core/Geometry/Matrix3D.h>
#include <Core/Materials/Appearance.h>
#include <Core/Materials/Appearances.h>
#include <Core/Materials/MaterialLibraries.h>
#include <Core/Materials/MaterialLibrary.h>
#include <Core/UserInterface/UserInterface.h>
#include <Core/UserInterface/CommandCreatedEventHandler.h>
#include <Core/UserInterface/CommandCreatedEvent.h>
#include <Core/UserInterface/CommandCreatedEventArgs.h>
#include <Core/UserInterface/CommandEvent.h>
#include <Core/UserInterface/CommandEventArgs.h>
#include <Core/UserInterface/CommandEventHandler.h>
#include <Core/UserInterface/InputChangedEvent.h>
#include <Core/UserInterface/InputChangedEventArgs.h>
#include <Core/UserInterface/InputChangedEventHandler.h>
#include <Core/UserInterface/Command.h>
#include <Core/UserInterface/CommandDefinition.h>
#include <Core/UserInterface/CommandDefinitions.h>
#include <Core/UserInterface/CommandInputs.h>
#include <Core/UserInterface/ValueCommandInput.h>
#include <Core/UserInterface/StringValueCommandInput.h>
#include <Core/UserInterface/BoolValueCommandInput.h>
#include <Core/UserInterface/ButtonRowCommandInput.h>
#include <Core/UserInterface/DropDownCommandInput.h>
#include <Core/UserInterface/Selection.h>
#include <Core/UserInterface/SelectionCommandInput.h>
#include <Core/UserInterface/FloatSliderCommandInput.h>
#include <Core/UserInterface/IntegerSliderCommandInput.h>
#include <Core/UserInterface/FloatSpinnerCommandInput.h>
#include <Core/UserInterface/GroupCommandInput.h>
#include <Core/UserInterface/IntegerSpinnerCommandInput.h>
#include <Core/UserInterface/ListItems.h>
#include <Core/UserInterface/ListItem.h>
#include <Core/UserInterface/DistanceValueCommandInput.h>
#include <Core/UserInterface/TableCommandInput.h>

#include <CAM/CAM/CAM.h>

#include <Fusion/BRep/BRepBody.h>
#include <Fusion/Components/Component.h>
#include <Fusion/Fusion/Design.h>
//#include <Fusion/Graphics/CustomGraphicsEntity.h>
#include <Fusion/Graphics/CustomGraphicsAppearanceColorEffect.h>
#include <Fusion/Graphics/CustomGraphicsBasicMaterialColorEffect.h>
#include <Fusion/Graphics/CustomGraphicsBillBoard.h>
#include <Fusion/Graphics/CustomGraphicsSolidColorEffect.h>
#include <Fusion/Graphics/CustomGraphicsVertexColorEffect.h>
#include <Fusion/Graphics/CustomGraphicsViewPlacement.h>
#include <Fusion/Graphics/CustomGraphicsViewScale.h>
#include <Fusion/Graphics/CustomGraphicsBRepBody.h>
#include <Fusion/Graphics/CustomGraphicsCoordinates.h>
#include <Fusion/Graphics/CustomGraphicsCurve.h>
#include <Fusion/Graphics/CustomGraphicsGroup.h>
#include <Fusion/Graphics/CustomGraphicsGroups.h>
#include <Fusion/Graphics/CustomGraphicsLines.h>
#include <Fusion/Graphics/CustomGraphicsMesh.h>
#include <Fusion/Graphics/CustomGraphicsText.h>
#include <Fusion/Graphics/CustomGraphicsPointSet.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/SketchArc.h>
#include <Fusion/Sketch/SketchEllipticalArc.h>
#include <Fusion/Sketch/SketchCircle.h>
#include <Fusion/Sketch/SketchEllipse.h>
#include <Fusion/Sketch/SketchFittedSpline.h>
#include <Fusion/Sketch/SketchLine.h>

#include <sstream>
#include <map>
#include <algorithm>  
#include <math.h>

using namespace adsk::core;
using namespace adsk::fusion;
using namespace adsk::cam;

namespace {
    const double pi = 4.0* atan(1.0);
}

Ptr<Application> _app;
Ptr<UserInterface> _ui;
Ptr<Design> _des;
Ptr<CustomGraphicsGroups> _cgGroups;
Ptr<Point3D> _anchorPt;

// Global 
const std::string _commandId = "CustomGraphicsSample_CPP";
const std::string _colorEffect_solid_id = "SolidColorEfect";
const std::string _colorEffect_basicMaterial_id = "BasicMaterialColorEffect";
const std::string _colorEffect_appearance_id = "AppearanceColorEffect";
const std::string _colorEffect_vertex_id = "VertexColorEffect";
const std::string _pointSetImage = "resources/16x16.png";
const std::string _solidLine_id = "Solid Line";
const std::string _centerLine_id = "Center Line";
const std::string _dashedLine_id = "Dashed Line";
const std::string _dotLine_id = "Dot Line";
const std::string _phantomLine_id = "Phantom Line";
const std::string _tracksLine_id = "Tracks Line";
const std::string _zigZagLine_id = "ZigZag Line";
int _numTeeth = 5;
double _thickness = 0.5 * 2.54;
double _scaleFactor = 10; // _scaleFactor is used to limit the size of pixel-scaled model

// Global command inputs
Ptr<DropDownCommandInput> _customGraphicsObj;
Ptr<DropDownCommandInput> _colorEffects;
Ptr<IntegerSliderCommandInput> _red;
Ptr<IntegerSliderCommandInput> _green;
Ptr<IntegerSliderCommandInput> _blue;
Ptr<FloatSliderCommandInput> _opacity;
Ptr<FloatSliderCommandInput> _glossiness;
Ptr<SelectionCommandInput> _selection;
Ptr<DistanceValueCommandInput> _transform;
Ptr<DropDownCommandInput> _materialLibList;
Ptr<DropDownCommandInput> _appearanceList;
Ptr<StringValueCommandInput> _appearanceFilter;
//Ptr<StringValueCommandInput> _text;
Ptr<TableCommandInput> _coordTable;
Ptr<BoolValueCommandInput> _add;
Ptr<BoolValueCommandInput> _addStrip;
Ptr<BoolValueCommandInput> _delete;
Ptr<BoolValueCommandInput> _isLineStrip;
Ptr<DropDownCommandInput> _lineStylePattern;
Ptr<IntegerSliderCommandInput> _lineStyleWeight;
Ptr<IntegerSliderCommandInput> _lineStyleScale;

Ptr<GroupCommandInput> _viewPlacementGroup;
Ptr<ButtonRowCommandInput> _viewCorner;
Ptr<GroupCommandInput> _viewScaleGroup;
Ptr<FloatSliderCommandInput> _pixelScale;
Ptr<GroupCommandInput> _billboardingGroup;
Ptr<ButtonRowCommandInput> _billboardingStyle;

// Create the command definition.
static Ptr<CommandDefinition> createCommandDefinition()
{
 Ptr<CommandDefinitions> commandDefinitions = _ui->commandDefinitions();
 if (!commandDefinitions)
  return nullptr;

 // Be fault tolerant in case the command is already added.
 Ptr<CommandDefinition> cmDef = commandDefinitions->itemById(_commandId);
 if (!cmDef)
 {
  cmDef = commandDefinitions->addButtonDefinition(_commandId,
   "CustomGraphicsSample",
   "Custom Graphics Sample");
 }

 return cmDef;
}

// Add a row representing a coordinate (x, y, z)
static void addRow(Ptr<TableCommandInput> tableInput)
{
 if (!tableInput)
  return;

 static size_t rowNumber = 0;
 std::stringstream rowId;
 rowId << rowNumber;
 Ptr<CommandInputs> tableChildInputs = tableInput->commandInputs();
 Ptr<CommandInput> xValueInput = tableChildInputs->addValueInput(tableInput->id() + "_x" + rowId.str(), "Value", "cm", ValueInput::createByReal(static_cast<double>(rowNumber)));
 Ptr<CommandInput> yValueInput = tableChildInputs->addValueInput(tableInput->id() + "_y" + rowId.str(), "Value", "cm", ValueInput::createByReal(static_cast<double>(rowNumber)));
 Ptr<CommandInput> zValueInput = tableChildInputs->addValueInput(tableInput->id() + "_z" + rowId.str(), "Value", "cm", ValueInput::createByReal(static_cast<double>(rowNumber)));

 int row = tableInput->rowCount();
 tableInput->addCommandInput(xValueInput, row, 0);
 tableInput->addCommandInput(yValueInput, row, 1);
 tableInput->addCommandInput(zValueInput, row, 2);

 rowNumber = rowNumber + 1;
}

static void addLineStrip(Ptr<TableCommandInput> tableInput)
{
 if (!tableInput)
  return;

 static size_t stripNumber = 0;
 std::stringstream stripId;
 stripId << stripNumber;

 Ptr<CommandInputs> tableChildInputs = tableInput->commandInputs();
 Ptr<StringValueCommandInput> strInput = tableChildInputs->addStringValueInput(tableInput->id() + "_strip" + stripId.str(), "Line Strip", "-- Line Strip --");
 if (!strInput)
  return;
 strInput->isReadOnly(true);

 int row = tableInput->rowCount();
 tableInput->addCommandInput(strInput, row, 0, 0, 2);

 stripNumber = stripNumber + 1;
}

static void replaceItems(Ptr<DropDownCommandInput> cmdInput, std::vector<std::string> newItems)
{
 if (!cmdInput)
  return;
 Ptr<ListItems> listItems = cmdInput->listItems();
 if (!listItems)
  return;
 listItems->clear();
 Ptr<ListItem> itemNone = listItems->add("None", true);
 if (!itemNone)
  return;
 itemNone->isSelected(true);

 if (newItems.size() > 0) {
  for (std::string str : newItems) {
   listItems->add(str, false);
  }
  if (listItems->count() > 1) {
   Ptr<ListItem> item1 = listItems->item(1);
   Ptr<ListItem> item0 = listItems->item(0); // item "None"
   if (!item1 || !item0)
    return;
   item1->isSelected(true);
   item0->deleteMe();
  }
 }
}

static std::vector<std::string> getAppearancesFromLib(const std::string& libName, const std::string& filterExp)
{
 std::vector<std::string> appearanceList;

 static std::map<std::string, std::vector<std::string>> appearanceMap;
 if (appearanceMap.find(libName) != appearanceMap.end()) {
  appearanceList = appearanceMap[libName];
 }
 else {
  if (!_app)
   return std::vector<std::string>();

  // get appearances according to libName
  Ptr<MaterialLibraries> matLibs = _app->materialLibraries();
  if (!matLibs)
   return std::vector<std::string>();
  Ptr<MaterialLibrary> matLib = matLibs->itemByName(libName);
  if (!matLib)
   return std::vector<std::string>();
  Ptr<Appearances> appearances = matLib->appearances();
  if (!appearances)
   return std::vector<std::string>();

  for (int i = 0; i < appearances->count(); ++i) {
   if(Ptr<Appearance> appear = appearances->item(i))
    appearanceList.push_back(appear->name());
  }
  appearanceMap[libName] = appearanceList;
 }

 // apply filter
 std::string lowerFilterExp(filterExp);
 if (!filterExp.empty()) {
  std::transform(filterExp.begin(), filterExp.end(), lowerFilterExp.begin(), ::tolower);
  std::vector<std::string> filteredList;
  for (std::string appearanceName : appearanceList) {
   std::string lowerName(appearanceName);
   std::transform(appearanceName.begin(), appearanceName.end(), lowerName.begin(), ::tolower);
   if (lowerName.find(lowerFilterExp) != std::string::npos) {
    filteredList.push_back(appearanceName);
   }
  }
  appearanceList = filteredList;
 }

 return appearanceList;
}

static bool hasAppearance(Ptr<MaterialLibrary> lib)
{
 if (!lib)
  return false;
 Ptr<Appearances> appearances = lib->appearances();
 if (!appearances || appearances->count() == 0)
  return false;

 return true;
}

static std::vector<std::string> getMaterialLibNames()
{
 if (!_app)
  return std::vector<std::string>();
 Ptr<MaterialLibraries> libs = _app->materialLibraries();
 if (!libs)
  return std::vector<std::string>();

 std::vector<std::string> libNames;
 for (int i = 0; i < libs->count(); ++i) {
  if (Ptr<MaterialLibrary> lib = libs->item(i)) {
   if (hasAppearance(lib)) {
    libNames.push_back(lib->name());
   }
  }
 }
 return libNames;
}

static Ptr<Appearance> getAppearance(const std::string& libName, const std::string& appearanceName)
{
 if (appearanceName.empty() || appearanceName == "None" || !_app)
  return nullptr;

 if (!_des)
  return nullptr;

 Ptr<Appearances> appearances = _des->appearances();
 if (appearances) {
  Ptr<Appearance> appearance = appearances->itemByName(appearanceName);
  if (appearance)
   return appearance;
 }

 Ptr<MaterialLibraries> libs = _app->materialLibraries();
 if (!libs)
  return nullptr;
 Ptr<MaterialLibrary> lib = libs->itemByName(libName);
 if (!lib)
  return nullptr;
 appearances = lib->appearances();
 if (!appearances)
  return nullptr;
 Ptr<Appearance> appearance = appearances->itemByName(appearanceName);
 return appearance;
}

static void applyLinesProperties(Ptr<CustomGraphicsLines> cgLines)
{
 if (!cgLines)
  return;
 if (_lineStylePattern && _lineStylePattern->selectedItem()) {
  if (_lineStylePattern->selectedItem()->name() == _solidLine_id)
   cgLines->lineStylePattern(continuousLineStylePattern);
  else if (_lineStylePattern->selectedItem()->name() == _centerLine_id)
   cgLines->lineStylePattern(centerLineStylePattern);
  else if (_lineStylePattern->selectedItem()->name() == _dashedLine_id)
   cgLines->lineStylePattern(dashedLineStylePattern);
  else if (_lineStylePattern->selectedItem()->name() == _dotLine_id)
   cgLines->lineStylePattern(dotLineStylePattern);
  else if (_lineStylePattern->selectedItem()->name() == _dashedLine_id)
   cgLines->lineStylePattern(dashedLineStylePattern);
  else if (_lineStylePattern->selectedItem()->name() == _phantomLine_id)
   cgLines->lineStylePattern(phantomLineStylePattern);
  else if (_lineStylePattern->selectedItem()->name() == _tracksLine_id)
   cgLines->lineStylePattern(tracksLineStylePattern);
  else if (_lineStylePattern->selectedItem()->name() == _zigZagLine_id)
   cgLines->lineStylePattern(zigzagLineStylePattern);
 }

 if (_lineStyleWeight)
  cgLines->weight(static_cast<float>(_lineStyleWeight->valueOne()));
 if (_lineStyleScale)
  cgLines->lineStyleScale(static_cast<float>(_lineStyleScale->valueOne()));
}

static void applyColorEffect(Ptr<CustomGraphicsEntity> cgEnt)
{
 if (!_des)
  return;
 if (!_colorEffects)
  return;

 Ptr<ListItem> selected = _colorEffects->selectedItem();
 if (!selected)
  return;

 std::string colorEffectName = selected->name();
 Ptr<CustomGraphicsColorEffect> colorEffect = nullptr;
 if (colorEffectName == _colorEffect_solid_id) {
  int red = 255, green = 0, blue = 0;
  if (_red)
   red = _red->valueOne();
  if (_green)
   green = _green->valueOne();
  if (_blue)
   blue = _blue->valueOne();

  if (Ptr<Color> solidColor = Color::create(red, green, blue, 255)) {
   colorEffect = CustomGraphicsSolidColorEffect::create(solidColor);
  }
 }
 else if (colorEffectName == _colorEffect_basicMaterial_id) {
  Ptr<Color> diffuseColor = Color::create(0, 255, 0, 255);
  Ptr<Color> ambientColor = Color::create(255, 0, 0, 255);
  Ptr<Color> specularColor = Color::create(0, 0, 255, 255);
  Ptr<Color> emissiveColor = Color::create(0, 0, 0, 255);
  double glossiness = 5.0, opacity = 1.0;
  if (_glossiness)
   glossiness = _glossiness->valueOne();
  if (_opacity)
   opacity = _opacity->valueOne();
  if (diffuseColor && ambientColor && specularColor && emissiveColor) {
   colorEffect = CustomGraphicsBasicMaterialColorEffect::create(diffuseColor, ambientColor, specularColor, emissiveColor, glossiness, opacity);
  }
 }
 else if (colorEffectName == _colorEffect_appearance_id) {
  if (!_appearanceList || !_materialLibList)
   return;
  Ptr<ListItem> appearanceSelected = _appearanceList->selectedItem();
  Ptr<ListItem> libSelected = _materialLibList->selectedItem();
  if (!appearanceSelected || !libSelected)
   return;
  std::string appearanceName = appearanceSelected->name();
  std::string libName = libSelected->name();
  Ptr<Appearance> appearance = getAppearance(libName, appearanceName);
  if (appearance) {
   Ptr<Appearances> desAppearances = _des->appearances();
   if (!desAppearances)
    return;
   if (!desAppearances->itemByName(appearanceName)) {
    appearance = desAppearances->addByCopy(appearance, appearanceName);
   }
   colorEffect = CustomGraphicsAppearanceColorEffect::create(appearance);
  }
 }
 else if (colorEffectName == _colorEffect_vertex_id) {
  colorEffect = CustomGraphicsVertexColorEffect::create();
 }

 if (colorEffect && cgEnt)
  cgEnt->color(colorEffect);
}

static void getCoordinatesFromTable(Ptr<TableCommandInput> tableInput, std::vector<double>& vecCoords, std::vector<int>& vecStripLen)
{
 vecCoords.clear();
 vecStripLen.clear();
 if (_coordTable) {
  int stripLen = 0;
  for (int i = 0; i < _coordTable->rowCount(); ++i) {
   if (Ptr<ValueCommandInput> xValueInput = tableInput->getInputAtPosition(i, 0)) {
    Ptr<ValueCommandInput> yValueInput = tableInput->getInputAtPosition(i, 1);
    Ptr<ValueCommandInput> zValueInput = tableInput->getInputAtPosition(i, 2);
    if (yValueInput && zValueInput) {
     vecCoords.push_back(xValueInput->value());
     vecCoords.push_back(yValueInput->value());
     vecCoords.push_back(zValueInput->value());
     ++stripLen;
    }
   }
   else if (Ptr<StringValueCommandInput> lineStripInput = tableInput->getInputAtPosition(i, 0)) {
    vecStripLen.push_back(stripLen);
    stripLen = 0;
   }
  }
  vecStripLen.push_back(stripLen);
 }
}

static void changeColorEffectVisibility(const std::string& strColorEffectName)
{
 if (_red)
  _red->isVisible(false);
 if (_green)
  _green->isVisible(false);
 if (_blue)
  _blue->isVisible(false);
 if (_opacity)
  _opacity->isVisible(false);
 if (_glossiness)
  _glossiness->isVisible(false);
 if (_appearanceList)
  _appearanceList->isVisible(false);
 if (_materialLibList)
  _materialLibList->isVisible(false);
 if (_appearanceFilter)
  _appearanceFilter->isVisible(false);

 if (strColorEffectName == _colorEffect_solid_id) {
  if (_red)
   _red->isVisible(true);
  if (_green)
   _green->isVisible(true);
  if (_blue)
   _blue->isVisible(true);
 }
 else if (strColorEffectName == _colorEffect_basicMaterial_id) {
  if (_opacity)
   _opacity->isVisible(true);
  if (_glossiness)
   _glossiness->isVisible(true);
 }
 else if (strColorEffectName == _colorEffect_appearance_id) {
  if (_appearanceList)
   _appearanceList->isVisible(true);
  if (_materialLibList)
   _materialLibList->isVisible(true);
  if (_appearanceFilter)
   _appearanceFilter->isVisible(true);
 }
}

static void changeLineStyleInputsVisibility(const std::string& patternName)
{
 if (patternName == _solidLine_id)
  _lineStyleScale->isVisible(false);
 else
  _lineStyleScale->isVisible(true);
}

static void changeCGObjVisibility(const std::string& strObjName)
{
 if (_colorEffects) {
  if (Ptr<ListItems> listItems = _colorEffects->listItems()) {
   listItems->clear();
   listItems->add(_colorEffect_solid_id, true);
   listItems->add(_colorEffect_basicMaterial_id, false);
   listItems->add(_colorEffect_appearance_id, false);
  }
  _colorEffects->isVisible(false);
 }
 if (_selection) {
  _selection->clearSelection();
  _selection->clearSelectionFilter();
  _selection->setSelectionLimits(0, 0);
  _selection->isVisible(false);
  _selection->isEnabled(false);
 }
 //if (_text)
 // _text->isVisible(false);
 if (_coordTable)
  _coordTable->isVisible(false);
 if (_isLineStrip)
  _isLineStrip->isVisible(false);
 if (_lineStylePattern)
  _lineStylePattern->isVisible(false);
 if (_lineStyleWeight)
  _lineStyleWeight->isVisible(false);
 if (_lineStyleScale)
  _lineStyleScale->isVisible(false);

 changeColorEffectVisibility("None");

 _viewPlacementGroup->isVisible(false);
 _viewScaleGroup->isVisible(false);
 _billboardingGroup->isVisible(false);

 if (strObjName == "Mesh") {
  if (_colorEffects) {
   _colorEffects->isVisible(true);
   if (Ptr<ListItems> listItems = _colorEffects->listItems()) {
    listItems->add(_colorEffect_vertex_id, false);
   }
   changeColorEffectVisibility(_colorEffect_solid_id);
   _viewPlacementGroup->isVisible(true);
   _viewScaleGroup->isVisible(true);
   _billboardingGroup->isVisible(true);
  }
 }
 else if (strObjName == "Lines") {
  changeColorEffectVisibility(_colorEffect_solid_id);
  _lineStylePattern->isVisible(true);
  _lineStyleWeight->isVisible(true);
  if (_lineStylePattern->selectedItem()->name() == _solidLine_id) {
   _lineStyleScale->isVisible(true);
  }
  _viewPlacementGroup->isVisible(true);
  _viewScaleGroup->isVisible(true);
  _billboardingGroup->isVisible(true);
 }
 else if (strObjName == "Curve") {
  if (_selection) {
   _selection->isVisible(true);
   _selection->isEnabled(true);
   _selection->tooltip("select a sketch curve");
   _selection->commandPrompt("select a sketch curve");
   _selection->addSelectionFilter("SketchCurve");
   _selection->setSelectionLimits(1, 1);
   changeColorEffectVisibility(_colorEffect_solid_id);
   _lineStyleWeight->isVisible(true);
  }
  _viewPlacementGroup->isVisible(true);
  _viewScaleGroup->isVisible(true);
  _billboardingGroup->isVisible(true);
 }
 else if (strObjName == "BRep") {
  if (_colorEffects)
   _colorEffects->isVisible(true);
  if (_selection) {
   _selection->isVisible(true);
   _selection->isEnabled(true);
   _selection->tooltip("select a BRep body");
   _selection->commandPrompt("select a BRep body");
   _selection->addSelectionFilter("Bodies");
   _selection->setSelectionLimits(1, 1);
   changeColorEffectVisibility(_colorEffect_solid_id);
  }
  _viewPlacementGroup->isVisible(true);
  _viewScaleGroup->isVisible(true);
  _billboardingGroup->isVisible(true);
 }
 //else if (strObjName == "Text") {
 // if (_text)
 //  _text->isVisible(true);
 // changeColorEffectVisibility(_colorEffect_solid_id);
 //}
 else if (strObjName == "PointSet - Custom") {
  if (_coordTable)
   _coordTable->isVisible(true);
  if (_addStrip)
   _addStrip->isEnabled(false);
 }
 else if (strObjName == "Lines - Custom") {
  if (_coordTable)
   _coordTable->isVisible(true);
  if (_isLineStrip)
   _isLineStrip->isVisible(true);
  if (_addStrip)
   _addStrip->isEnabled(true);
  changeColorEffectVisibility(_colorEffect_solid_id);
  _lineStylePattern->isVisible(true);
  _lineStyleWeight->isVisible(true);
  if (_lineStylePattern->selectedItem()->name() == _solidLine_id) {
   _lineStyleScale->isVisible(true);
  }
 }
}

static Ptr<Vector2D> rotate2D(double rad, Ptr<Vector2D> vec)
{
 if (!vec)
  return Vector2D::create();
 double x = vec->x();
 double y = vec->y();

 double newX = x * cos(rad) - y * sin(rad);
 double newY = x * sin(rad) + y * cos(rad);
 return Vector2D::create(newX, newY);
}

static void calculateCoordinates(int numTeeth,
 /*out*/std::vector<int>& rPts0, /*out*/std::vector<int>& hPts0,/*out*/std::vector<int>& pPts0,/*out*/std::vector<int>& oPts0,
 /*out*/std::vector<int>& rPts1, /*out*/std::vector<int>& hPts1,/*out*/std::vector<int>& pPts1,/*out*/std::vector<int>& oPts1,
 /*out*/std::vector<double>& vecCoords, /*out*/std::vector<short>& vecColors)
{
 if (numTeeth < 3)
  return;
 // holeDia < rootDia < pitchDia < outsideDia
 double holeDia = 0.5 * 2.54, diametralPitch = 2 / 2.54;
 double pitchDia = _numTeeth / diametralPitch;
 double dedendum = 1.157 / diametralPitch;
 if (fabs((20 * (pi / 180)) - diametralPitch) < 1e-6) {
  double circularPitch = pi / diametralPitch;
  if (circularPitch >= 20)
   dedendum = 1.25 / diametralPitch;
  else
   dedendum = (1.2 / diametralPitch) + (.002 * 2.54);
 }
 double rootDia = pitchDia - (2 * dedendum);
 double outsideDia = (_numTeeth + 2) / diametralPitch;
 
 Ptr<Vector2D> vecRootRadi = Vector2D::create(rootDia / 2, 0);
 Ptr<Vector2D> vecHoleRadi = Vector2D::create(holeDia / 2, 0);
 Ptr<Vector2D> vecPitchRadi = Vector2D::create(pitchDia / 2, 0);
 Ptr<Vector2D> vecOutRadi = Vector2D::create(outsideDia / 2, 0);
 double unitRadian = pi / numTeeth;

 for (int i = 0; i < 2 * numTeeth; ++i) {
  if (Ptr<Vector2D> pos = rotate2D(unitRadian * (i - 0.5), vecRootRadi)) {
   double x = pos->x(), y = pos->y();
   rPts0.push_back(static_cast<int>(vecCoords.size() / 3));
   rPts1.push_back(static_cast<int>(vecCoords.size() / 3 + 1));
   vecCoords.insert(vecCoords.end(), { x, y, 0, x, y, _thickness });
   vecColors.insert(vecColors.end(), { 255,0,255,128, 255,0,255,128 });
  }
 }
 for (int i = 0; i < 2 * numTeeth; ++i) {
  if (Ptr<Vector2D> pos = rotate2D(unitRadian * (i - 0.5), vecHoleRadi)) {
   double x = pos->x(), y = pos->y();
   hPts0.push_back(static_cast<int>(vecCoords.size() / 3));
   hPts1.push_back(static_cast<int>(vecCoords.size() / 3 + 1));
   vecCoords.insert(vecCoords.end(), { x, y, 0, x, y, _thickness });
   vecColors.insert(vecColors.end(), { 255,0,0,128,  255,0,0,128 });
  }
 }
 for (int i = 0; i < 2 * numTeeth; ++i) {
  if (Ptr<Vector2D> pos = rotate2D(unitRadian * (i - 0.5), vecPitchRadi)) {
   double x = pos->x(), y = pos->y();
   pPts0.push_back(static_cast<int>(vecCoords.size() / 3));
   pPts1.push_back(static_cast<int>(vecCoords.size() / 3 + 1));
   vecCoords.insert(vecCoords.end(), { x, y, 0, x, y, _thickness });
   vecColors.insert(vecColors.end(), { 0,0,255,128, 0,0,255,128 });
  }
 }
 for (int i = 0; i < numTeeth; ++i) {
  if (Ptr<Vector2D> pos = rotate2D(unitRadian * i * 2, vecOutRadi)) {
   double x = pos->x(), y = pos->y();
   oPts0.push_back(static_cast<int>(vecCoords.size() / 3));
   oPts1.push_back(static_cast<int>(vecCoords.size() / 3 + 1));
   vecCoords.insert(vecCoords.end(), { x, y, 0, x, y, _thickness });
   vecColors.insert(vecColors.end(), { 0,255,255,128, 0,255,255,128 });
  }
 }
}

static std::vector<int> calculateStripLen(int numTeeth)
{
 if (numTeeth < 3)
  return std::vector<int>();

 std::vector<int> vecStripLen;
 for (int i = 0; i < numTeeth; ++i)
  vecStripLen.push_back(6);
 for (int i = 0; i < 2*numTeeth; ++i)
  vecStripLen.push_back(21);
 for (int i = 0; i < numTeeth; ++i)
  vecStripLen.push_back(24);
 for (int i = 0; i < 2*numTeeth; ++i)
  vecStripLen.push_back(6);
 return vecStripLen;
}

static std::vector<int> calculateTriangles(int numTeeth,
 std::vector<int> rPts0, std::vector<int> hPts0, std::vector<int> pPts0, std::vector<int> oPts0,
 std::vector<int> rPts1, std::vector<int> hPts1, std::vector<int> pPts1, std::vector<int> oPts1)
{
 if (numTeeth < 3)
  return std::vector<int>();

 std::vector<int> vertexIndexList;
 // triangles between teeth
 for (int i = 0; i < numTeeth; ++i) {
  int idx0 = (2 * i + 1) % (2 * numTeeth);
  int idx1 = (2 * i + 2) % (2 * numTeeth);
  int rPtA0 = rPts0[idx0];
  int rPtB0 = rPts0[idx1];
  int rPtA1 = rPts1[idx0];
  int rPtB1 = rPts1[idx1];
  vertexIndexList.insert(vertexIndexList.end(), { rPtA0,rPtB0,rPtB1, rPtB1,rPtA1,rPtA0 });
 }

 // triangles on surface0
 for (int i = 0; i < numTeeth; ++i) {
  int rPtA = rPts0[i * 2];
  int rPtB = rPts0[i * 2 + 1];
  int rPtC = rPts0[(i * 2 + 2) % (2 * numTeeth)];
  int hPtA = hPts0[i * 2];
  int hPtB = hPts0[i * 2 + 1];
  int hPtC = hPts0[(i * 2 + 2) % (2 * numTeeth)];
  int pPtA = pPts0[i * 2];
  int pPtB = pPts0[i * 2 + 1];
  int oPt = oPts0[i];
  vertexIndexList.insert(vertexIndexList.end(), 
  { hPtB,hPtC,rPtC, rPtC,rPtB,hPtB,
   rPtA,rPtB,pPtB, pPtB,pPtA,rPtA,
   hPtA,hPtB,rPtB, rPtB,rPtA,hPtA,
   pPtA,pPtB,oPt });
 }

 // triangles on surface1
 for (int i = 0; i < numTeeth; ++i) {
  int rPtA = rPts1[i * 2];
  int rPtB = rPts1[i * 2 + 1];
  int rPtC = rPts1[(i * 2 + 2) % (2 * numTeeth)];
  int hPtA = hPts1[i * 2];
  int hPtB = hPts1[i * 2 + 1];
  int hPtC = hPts1[(i * 2 + 2) % (2 * numTeeth)];
  int pPtA = pPts1[i * 2];
  int pPtB = pPts1[i * 2 + 1];
  int oPt = oPts1[i];
  vertexIndexList.insert(vertexIndexList.end(),
  { hPtC,hPtB,rPtB, rPtB,rPtC,hPtC,
   rPtB,rPtA,pPtA, pPtA,pPtB,rPtB,
   hPtB,hPtA,rPtA, rPtA,rPtB,hPtB,
   pPtB,pPtA,oPt });
 }

 // triangles on teeth
 for (int i = 0; i < numTeeth; ++i) {
  int rPtA0 = rPts0[i * 2];
  int rPtB0 = rPts0[i * 2 + 1];
  int pPtA0 = pPts0[i * 2];
  int pPtB0 = pPts0[i * 2 + 1];
  int rPtA1 = rPts1[i * 2];
  int rPtB1 = rPts1[i * 2 + 1];
  int pPtA1 = pPts1[i * 2];
  int pPtB1 = pPts1[i * 2 + 1];
  int oPt0 = oPts0[i];
  int oPt1 = oPts1[i];
  vertexIndexList.insert(vertexIndexList.end(),
  { rPtA1, rPtA0, pPtA0, pPtA0, pPtA1, rPtA1,
   pPtA1, pPtA0, oPt0, oPt0, oPt1, pPtA1,
   rPtB0, rPtB1, pPtB1, pPtB1, pPtB0, rPtB0,
   pPtB0, pPtB1, oPt1, oPt1, oPt0, pPtB0 });
 }

 // triangles on inner face
 for (int i = 0; i < 2 * numTeeth; ++i) {
  int hPtA0 = hPts0[i];
  int hPtB0 = hPts0[(i + 1) % (2 * numTeeth)];
  int hPtA1 = hPts1[i];
  int hPtB1 = hPts1[(i + 1) % (2 * numTeeth)];
  vertexIndexList.insert(vertexIndexList.end(),
  { hPtA1,hPtB1,hPtB0, hPtB0,hPtA0,hPtA1 });
 }

 return vertexIndexList;
}

static Ptr<CustomGraphicsMesh> drawMesh(const Ptr<CustomGraphicsGroup>& cgGroup)
{
 //  Calculate mesh coordinates
 std::vector<int> rPts0, hPts0, pPts0, oPts0, rPts1, hPts1, pPts1, oPts1;
 std::vector<short> vecColors;
 std::vector<double> vecCoords;
 calculateCoordinates(_numTeeth, rPts0, hPts0, pPts0, oPts0, rPts1, hPts1, pPts1, oPts1, vecCoords, vecColors);
 Ptr<CustomGraphicsCoordinates> coordinates = CustomGraphicsCoordinates::create(vecCoords);
 if (!coordinates)
  return nullptr;
 coordinates->colors(vecColors);

 // Calculate mesh triangles
 std::vector<int> vertexIndexList = calculateTriangles(_numTeeth, rPts0, hPts0, pPts0, oPts0, rPts1, hPts1, pPts1, oPts1);
 // Add Custom Graphics mesh
 if (!cgGroup)
  return nullptr;
 return cgGroup->addMesh(coordinates, vertexIndexList, std::vector<double>(), std::vector<int>());
}

static Ptr<CustomGraphicsLines> drawLines(const Ptr<CustomGraphicsGroup>& cgGroup)
{
 if (!cgGroup)
  return nullptr;

 //  Calculate lines coordinates
 std::vector<int> rPts0, hPts0, pPts0, oPts0, rPts1, hPts1, pPts1, oPts1;
 std::vector<short> vecColors;
 std::vector<double> vecCoords;
 calculateCoordinates(_numTeeth, rPts0, hPts0, pPts0, oPts0, rPts1, hPts1, pPts1, oPts1, vecCoords, vecColors);
 Ptr<CustomGraphicsCoordinates> coordinates = CustomGraphicsCoordinates::create(vecCoords);
 if (!coordinates)
  return nullptr;

 // Calculate lines triangles
 std::vector<int> vertexIndexList = calculateTriangles(_numTeeth, rPts0, hPts0, pPts0, oPts0, rPts1, hPts1, pPts1, oPts1);

 // Calculate lines strip length
 std::vector<int> vecStripLen = calculateStripLen(_numTeeth);

 Ptr<CustomGraphicsLines> cgLines = cgGroup->addLines(coordinates, vertexIndexList, true, vecStripLen);
 return cgLines;
}

static Ptr<CustomGraphicsPointSet> drawPointSet(const Ptr<CustomGraphicsGroup>& cgGroup)
{
 if (!cgGroup)
  return nullptr;

 //  Calculate coordinates
 std::vector<int> rPts0, hPts0, pPts0, oPts0, rPts1, hPts1, pPts1, oPts1;
 std::vector<short> vecColors;
 std::vector<double> vecCoords;
 calculateCoordinates(_numTeeth, rPts0, hPts0, pPts0, oPts0, rPts1, hPts1, pPts1, oPts1, vecCoords, vecColors);
 Ptr<CustomGraphicsCoordinates> coordinates = CustomGraphicsCoordinates::create(vecCoords);
 if (!coordinates)
  return nullptr;

 return cgGroup->addPointSet(coordinates, std::vector<int>(), UserDefinedCustomGraphicsPointType, _pointSetImage);
}

// CommandExecuted event handler.
class OnExecuteEventHandler : public adsk::core::CommandEventHandler
{
public:
 void notify(const Ptr<CommandEventArgs>& eventArgs) override
 {
  //  get selection entity first since it's fragile and any creation/edit operations will clear the selection.
  Ptr<Base> selEntity = nullptr;
  if (_selection && _selection->selectionCount() > 0) {
   if (Ptr<Selection> sel0 = _selection->selection(0)) {
    selEntity = sel0->entity();
   }
  }

  if (_customGraphicsObj) {
   if (!_cgGroups)
    return;
   Ptr<CustomGraphicsGroup> cgGroup = _cgGroups->add();
   if (!cgGroup)
    return;

   if (!_anchorPt)
    _anchorPt = Point3D::create();

   Ptr<CustomGraphicsEntity> cgEnt = nullptr;
   Ptr<ListItem> selectedCGObj = _customGraphicsObj->selectedItem();
   if (!selectedCGObj)
    return;
   std::string cgObjName = selectedCGObj->name();
   if (cgObjName == "Mesh") {
    cgEnt = drawMesh(cgGroup);
    _anchorPt->setWithArray({ 0, 0, _thickness / 2 });
   }
   else if (cgObjName == "Lines") {
    cgEnt = drawLines(cgGroup);
    _anchorPt->setWithArray({ 0, 0, _thickness / 2 });
    applyLinesProperties(cgEnt);
   }
   else if (cgObjName == "PointSet") {
    cgEnt = drawPointSet(cgGroup);
   }
   else if (cgObjName == "BRep") {
    if (Ptr<BRepBody> body = selEntity->cast<BRepBody>()) {
     cgEnt = cgGroup->addBRepBody(body);
    }
   }
   else if (cgObjName == "Curve") {
    if (Ptr<SketchCurve> skCurve = selEntity->cast<SketchCurve>()) {
     if (Ptr<Sketch> sk = skCurve->parentSketch()) {
      Ptr<Curve3D> curv = nullptr;
      if (Ptr<SketchArc> skArc = skCurve->cast<SketchArc>())
       curv = skArc->geometry();
      else if (Ptr<SketchEllipticalArc> skEllipArc = skCurve->cast<SketchEllipticalArc>())
       curv = skEllipArc->geometry();
      else if (Ptr<SketchCircle> skCircle = skCurve->cast<SketchCircle>())
       curv = skCircle->geometry();
      else if (Ptr<SketchEllipse> skEllipse = skCurve->cast<SketchEllipse>())
       curv = skEllipse->geometry();
      else if (Ptr<SketchLine> skLine = skCurve->cast<SketchLine>())
       curv = skLine->geometry();
      else if (Ptr<SketchFittedSpline> skSpline = skCurve->cast<SketchFittedSpline>())
       curv = skSpline->geometry();

      if (curv) {
       curv->transformBy(sk->transform());
       Ptr<CustomGraphicsCurve> cgCurve = cgGroup->addCurve(curv);
       cgEnt = cgCurve;
       if (cgCurve)
        cgCurve->weight(static_cast<float>(_lineStyleWeight->valueOne()));
      }
     }
    }
   }
   //else if (cgObjName == "Text") {
   // if (_text) {
   //  cgEnt = cgGroup->addText(_text->value(), "Test", 10, Point3D::create());
   // }
   //}
   else if (cgObjName == "PointSet - Custom") {
    if (_coordTable) {
     std::vector<double> vecCoords;
     std::vector<int> vecStripLen;
     getCoordinatesFromTable(_coordTable, vecCoords, vecStripLen);
     Ptr<CustomGraphicsCoordinates> coordinates = CustomGraphicsCoordinates::create(vecCoords);
     cgEnt = cgGroup->addPointSet(coordinates, std::vector<int>(), UserDefinedCustomGraphicsPointType, _pointSetImage);
    }
   }
   else if (cgObjName == "Lines - Custom") {
    if (_coordTable) {
     std::vector<double> vecCoords;
     std::vector<int> vecStripLen;
     getCoordinatesFromTable(_coordTable, vecCoords, vecStripLen);
     Ptr<CustomGraphicsCoordinates> coordinates = CustomGraphicsCoordinates::create(vecCoords);
     bool isLineStrip = true;
     if (_isLineStrip)
      isLineStrip = _isLineStrip->value();
     cgEnt = cgGroup->addLines(coordinates, std::vector<int>(), isLineStrip, vecStripLen);
     applyLinesProperties(cgEnt);
    }
   }

   // add attributes to the custom graphics entity
   if (cgEnt) { 
    // color effect
    if (!cgEnt->cast<CustomGraphicsPointSet>()) // do not apply effect to point set node
     applyColorEffect(cgEnt);
    // transform
    Ptr<Matrix3D> transMat = Matrix3D::create();
    double transformDistance = 1.0;
    if (_transform)
     transformDistance = _transform->value();
    Ptr<Point3D> origin = Point3D::create(transformDistance, 0, 0);
    if (transMat && origin) {
     transMat->setWithCoordinateSystem(origin, Vector3D::create(1, 0, 0), Vector3D::create(0, 1, 0), Vector3D::create(0, 0, 1));
     cgEnt->transform(transMat);
    }
    // calculate _scaleFactor and _anchorPt for viewPlacement, viewScale and billboarding attributes based on the bounding box of custom graphics entity
    if (Ptr<BoundingBox3D> bbox = cgEnt->boundingBox()) {
     Ptr<Point3D> maxPt = bbox->maxPoint();
     Ptr<Point3D> minPt = bbox->minPoint();
     if (maxPt && minPt) {
      _scaleFactor = 100 / minPt->distanceTo(maxPt);
      _anchorPt->setWithArray({ (minPt->x() + maxPt->x()) / 2, (minPt->y() + maxPt->y()) / 2, (minPt->z() + maxPt->z()) / 2 });
     }
    }
    // view placement
    if (_viewPlacementGroup && _viewPlacementGroup->isVisible() && _viewPlacementGroup->isEnabledCheckBoxChecked() && _viewCorner && _viewCorner->selectedItem()) 
    {
     Ptr<Point2D> viewPt = Point2D::create(100, 100);
     // upper left corner by default
     ViewCorners corner = ViewCorners::upperLeftViewCorner;
     Ptr<ListItem> selected = _viewCorner->selectedItem();
     if (selected->name() == "Upper Right")
      corner = ViewCorners::upperRightViewCorner;
     else if (selected->name() == "Lower Left")
      corner = ViewCorners::lowerLeftViewCorner;
     else if (selected->name() == "Lower Right")
      corner = ViewCorners::lowerRightViewCorner;
     Ptr<CustomGraphicsViewPlacement> attr = CustomGraphicsViewPlacement::create(_anchorPt, corner, viewPt);
     cgEnt->viewPlacement(attr);
    }
    // view scale
    if (_viewScaleGroup && _viewScaleGroup->isVisible() && _viewScaleGroup->isEnabledCheckBoxChecked() && _pixelScale)
    {
     Ptr<CustomGraphicsViewScale> attr = CustomGraphicsViewScale::create(_scaleFactor * _pixelScale->valueOne(), _anchorPt);
     cgEnt->viewScale(attr);
    }
    // billboarding
    if (_billboardingGroup && _billboardingGroup->isVisible() && _billboardingGroup->isEnabledCheckBoxChecked() && _billboardingStyle && _billboardingStyle->selectedItem())
    {
     //  screen style by default
     CustomGraphicsBillBoardStyles bbStyle = CustomGraphicsBillBoardStyles::ScreenBillBoardStyle;
     Ptr<ListItem> selected = _billboardingStyle->selectedItem();
     if (selected->name() == "Axis")
      bbStyle = CustomGraphicsBillBoardStyles::AxialBillBoardStyle;
     else if (selected->name() == "Right Reading")
      bbStyle = CustomGraphicsBillBoardStyles::RightReadingBillBoardStyle;
     Ptr<CustomGraphicsBillBoard> attr = CustomGraphicsBillBoard::create(_anchorPt);
     attr->axis(Vector3D::create(0, 1, 0));
     attr->billBoardStyle(bbStyle);
     cgEnt->billBoarding(attr);
    }
   }
  }
 }
};

// InputChange event handler.
class OnInputChangedEventHander : public adsk::core::InputChangedEventHandler
{
public:
 void notify(const Ptr<InputChangedEventArgs>& eventArgs) override
 {
  if (!eventArgs)
   return;

  Ptr<CommandInput> changedInput = eventArgs->input();
  if (!changedInput)
   return;

  std::string changedInputId = changedInput->id();
  if (changedInputId == _commandId + "_cgObj") {
   if (_customGraphicsObj) {
    if (Ptr<ListItem> selectedItem = _customGraphicsObj->selectedItem()) {
     changeCGObjVisibility(selectedItem->name());
    }
   }
  }
  else if (changedInputId == _commandId + "_colorEffects") {
   if (_colorEffects) {
    if (Ptr<ListItem> selectedItem = _colorEffects->selectedItem()) {
     changeColorEffectVisibility(selectedItem->name());
    }
   }
  }
  else if (changedInputId == _commandId + "_appearanceFilter" || changedInputId == _commandId + "_materialLib") {
   std::string libName(""), filterExp("");
   if (_materialLibList) {
    if (Ptr<ListItem> selectedItem = _materialLibList->selectedItem()) {
     libName = selectedItem->name();
    }
   }
   if (_appearanceFilter) {
    filterExp = _appearanceFilter->value();
   }
   std::vector<std::string> appearanceNames = getAppearancesFromLib(libName, filterExp);
   if (_appearanceList) {
    replaceItems(_appearanceList, appearanceNames);
   }
  }
  else if (_coordTable && changedInputId == _coordTable->id() + "_add") {
   addRow(_coordTable);
  }
  else if (_coordTable && changedInputId == _coordTable->id() + "_addStrip") {
   addLineStrip(_coordTable);
  }
  else if (_coordTable && changedInputId == _coordTable->id() + "_delete") {
   int selectedRowNo = _coordTable->selectedRow();
   if (selectedRowNo == -1) {
    _ui->messageBox("Select one row to delete");
   }
   else {
    _coordTable->deleteRow(selectedRowNo);
   }
  }
  else if (_lineStylePattern && changedInputId == _commandId + "_LSPattern") {
   changeLineStyleInputsVisibility(_lineStylePattern->selectedItem()->name());
  }
 }
};

// CommandDestroyed event handler
class OnDestroyEventHandler : public adsk::core::CommandEventHandler
{
public:
 void notify(const Ptr<CommandEventArgs>& eventArgs) override
 {
  adsk::terminate();
 }
};

// CommandCreated event handler.
class CommandCreatedEventHandler : public adsk::core::CommandCreatedEventHandler
{
public:
 void notify(const Ptr<CommandCreatedEventArgs>& eventArgs) override
 {
  if (eventArgs)
  {
   Ptr<Command> command = eventArgs->command();
   if (command)
   {
    Ptr<CommandEvent> onDestroy = command->destroy();
    if (!onDestroy)
     return;
    bool isOk = onDestroy->add(&onDestroyHandler_);
    if (!isOk)
     return;

    Ptr<InputChangedEvent> onInputChanged = command->inputChanged();
    if (!onInputChanged)
     return;
    isOk = onInputChanged->add(&onInputChangedHandler_);
    if (!isOk)
     return;

    Ptr<CommandEvent> onExecute = command->execute();
    if (!onExecute)
     return;
    isOk = onExecute->add(&onExecuteHandler_);
    if (!isOk)
     return;

    Ptr<CommandEvent> onExecutePtrview = command->executePreview();
    if (!onExecutePtrview)
     return;
    isOk = onExecutePtrview->add(&onExecuteHandler_);
    if (!isOk)
     return;

    Ptr<CommandInputs> inputs = command->commandInputs();
    if (!inputs)
     return;

    // menu for different kinds of custom graphics
    _customGraphicsObj = inputs->addDropDownCommandInput(_commandId + "_cgObj", "Custom Graphics Object", DropDownStyles::TextListDropDownStyle);
    if (_customGraphicsObj) {
     Ptr<ListItems> listItems = _customGraphicsObj->listItems();
     if (listItems) {
      listItems->add("Mesh", true);
      listItems->add("Lines", false);
      listItems->add("PointSet", false);
      listItems->add("Curve", false);
      listItems->add("BRep", false);
      //listItems->add("Text", false);
      listItems->add("Lines - Custom", false);
      listItems->add("PointSet - Custom", false);
     }
    }

    // coordinates table used by 'Lines - Custom' and 'PointSet - Custom'
    _coordTable = inputs->addTableCommandInput(_commandId + "_table", "Coordinates Table", 3, "1:1:1");
    if (_coordTable) {
     _coordTable->maximumVisibleRows(10);
     addRow(_coordTable);
     Ptr<CommandInput> addButtonInput = inputs->addBoolValueInput(_coordTable->id() + "_add", "Add", false, "", true);
     if (addButtonInput) {
      _coordTable->addToolbarCommandInput(addButtonInput);
      addButtonInput->isVisible(false);
     }
     Ptr<CommandInput> addStripButtonInput = inputs->addBoolValueInput(_coordTable->id() + "_addStrip", "Add Strip", false, "", true);
     if (addStripButtonInput) {
      _coordTable->addToolbarCommandInput(addStripButtonInput);
      addStripButtonInput->isVisible(false);
     }
     Ptr<CommandInput> deleteButtonInput = inputs->addBoolValueInput(_coordTable->id() + "_delete", "Delete", false, "", true);
     if (deleteButtonInput) {
      _coordTable->addToolbarCommandInput(deleteButtonInput);
      deleteButtonInput->isVisible(false);
     }
     _coordTable->isVisible(false);
    }

    // specific for 'Lines - Custom'
    _isLineStrip = inputs->addBoolValueInput(_commandId + "_isLineStrip", "Use LineStrip", true, "", true);
    if (_isLineStrip) {
     _isLineStrip->isVisible(false);
    }

    // color effects for custom graphics Mesh/BRep
    _colorEffects = inputs->addDropDownCommandInput(_commandId + "_colorEffects", "Color Effect", DropDownStyles::TextListDropDownStyle);
    if (_colorEffects) {
     Ptr<ListItems> listItems = _colorEffects->listItems();
     if (listItems) {
      listItems->add(_colorEffect_solid_id, true);
      listItems->add(_colorEffect_basicMaterial_id, false);
      listItems->add(_colorEffect_appearance_id, false);
      listItems->add(_colorEffect_vertex_id, false);
     }
    }

    // RGB for solid colors
    _red = inputs->addIntegerSliderCommandInput(_commandId + "_red", "Red", 0, 255);
    if (_red)
     _red->valueOne(255);
    _green = inputs->addIntegerSliderCommandInput(_commandId + "_green", "Green", 0, 255);
    if (_green)
     _green->valueOne(0);
    _blue = inputs->addIntegerSliderCommandInput(_commandId + "_blue", "Blue", 0, 255);
    if (_blue)
     _blue->valueOne(0);

    // specific for basic material color effect
    _glossiness = inputs->addFloatSliderCommandInput(_commandId + "_glossiness", "Glossiness", "", 0, 128);
    if (_glossiness) {
     _glossiness->valueOne(128);
     _glossiness->isVisible(false);
    }
    _opacity = inputs->addFloatSliderCommandInput(_commandId + "_opacity", "Opacity", "", 0, 1);
    if (_opacity) {
     _opacity->valueOne(1);
     _opacity->isVisible(false);
    }

    // for appearance color effect
    std::string defaultMatLibName("");
    _materialLibList = inputs->addDropDownCommandInput(_commandId + "_materialLib", "Material Library", DropDownStyles::TextListDropDownStyle);
    if (_materialLibList) {
     Ptr<ListItems> listItems = _materialLibList->listItems();
     if (listItems) {
      std::vector<std::string> matLibNames = getMaterialLibNames();
      for (std::string libName : matLibNames) {
       listItems->add(libName, false);
      }
      if (listItems->count() > 0) {
       Ptr<ListItem> item0 = listItems->item(0);
       if (item0) {
        item0->isSelected(true);
        defaultMatLibName = item0->name();
       }
      }
     }
     _materialLibList->isVisible(false);
    }
    _appearanceList = inputs->addDropDownCommandInput(_commandId + "_appearanceList", "Appearance", DropDownStyles::TextListDropDownStyle);
    if (_appearanceList && !defaultMatLibName.empty()) {
     Ptr<ListItems> listItems = _appearanceList->listItems();
     if (listItems) {
      std::vector<std::string> defaultAppearanceList = getAppearancesFromLib(defaultMatLibName, "");
      for (std::string appearanceName : defaultAppearanceList) {
       listItems->add(appearanceName, false);
      }
      if (listItems->count() > 0) {
       Ptr<ListItem> item0 = listItems->item(0);
       if (item0) {
        item0->isSelected(true);
       }
      }
     }
     _appearanceList->isVisible(false);
    }
    _appearanceFilter = inputs->addStringValueInput(_commandId + "_appearanceFilter", "Filter");
    if (_appearanceFilter) {
     _appearanceFilter->isVisible(false);
    }

    // selection input for custom graphics BRep/Curve
    _selection = inputs->addSelectionInput(_commandId + "_sel", "Selection", "");
    if (_selection) {
     _selection->setSelectionLimits(0, 1);
     _selection->isVisible(false);
     _selection->isEnabled(false);
    }

    //// for custom graphics text
    //_text = inputs->addStringValueInput(_commandId + "_text", "Text", "This is a text.");
    //if (_text) {
    // _text->isVisible(false);
    //}

    // transform for all custom graphics entity    
    if (Ptr<ValueInput> transformDistance = adsk::core::ValueInput::createByReal(0)) {
     _transform = inputs->addDistanceValueCommandInput(_commandId + "_transform", "Transform", transformDistance);
     if (_transform) {
      Ptr<Point3D> origin = adsk::core::Point3D::create(0, 0, 0);
      Ptr<Vector3D> direction = adsk::core::Vector3D::create(1, 0, 0);
      if (origin && direction) {
       _transform->setManipulator(origin, direction);
      }
     }
    }

    // for custom graphics line style pattern
    _lineStylePattern = inputs->addDropDownCommandInput(_commandId + "_LSPattern", "Line Style Pattern", DropDownStyles::TextListDropDownStyle);
    if (_lineStylePattern) {
     Ptr<ListItems> listItems = _lineStylePattern->listItems();
     if (listItems) {
      listItems->add(_solidLine_id, true);
      listItems->add(_centerLine_id, false);
      listItems->add(_dashedLine_id, false);
      listItems->add(_dotLine_id, false);
      listItems->add(_phantomLine_id, false);
      listItems->add(_tracksLine_id, false);
      listItems->add(_zigZagLine_id, false);
     }
     _lineStylePattern->isVisible(false);
    }

    // for line style weight
    _lineStyleWeight = inputs->addIntegerSliderCommandInput(_commandId + "_LSWeight", "Line Style Weight", 1, 20);
    _lineStyleWeight->valueOne(1);
    _lineStyleWeight->isVisible(false);

    // for line style scale
    _lineStyleScale = inputs->addIntegerSliderCommandInput(_commandId + "_LSScale", "Line Style Scale", 1, 100);
    _lineStyleScale->valueOne(10);
    _lineStyleScale->isVisible(false);

    // for view placement attribute
    _viewPlacementGroup = inputs->addGroupCommandInput(_commandId + "_VPGroup", "View Placement");
    if (_viewPlacementGroup) 
    {
     _viewPlacementGroup->isEnabledCheckBoxDisplayed(true);
     _viewPlacementGroup->isEnabledCheckBoxChecked(false);
     if (Ptr<CommandInputs> childInputs = _viewPlacementGroup->children())
     {
      _viewCorner = childInputs->addButtonRowCommandInput(_commandId + "_viewCorner", "corner", false);
      if(_viewCorner)
      {
       if (Ptr<ListItems> listItems = _viewCorner->listItems()) 
       {
        listItems->add("Upper Left", false, "./resources/upperLeft");
        listItems->add("Upper Right", false, "./resources/upperRight");
        listItems->add("Lower Left", false, "./resources/lowerLeft");
        listItems->add("Lower Right", false, "./resources/lowerRight");
       }
      }
     }
    }

    // for view scale attribute
    _viewScaleGroup = inputs->addGroupCommandInput(_commandId + "_VSGroup", "View Scale");
    if (_viewScaleGroup) 
    {
     _viewScaleGroup->isEnabledCheckBoxDisplayed(true);
     _viewScaleGroup->isEnabledCheckBoxChecked(false);
     if (Ptr<CommandInputs> childInputs = _viewScaleGroup->children())
     {
      _pixelScale = childInputs->addFloatSliderCommandInput(_commandId + "_pixelScale", "pixel scale", "", 0.5,5,false);
      if (_pixelScale) 
      {
       _pixelScale->valueOne(1);
       _pixelScale->setText("Smaller", "Larger");
      }
     }
    }

    // for billboarding attribute
    _billboardingGroup = inputs->addGroupCommandInput(_commandId + "_BBGroup", "Billboarding");
    if (_billboardingGroup) 
    {
     _billboardingGroup->isEnabledCheckBoxDisplayed(true);
     _billboardingGroup->isEnabledCheckBoxChecked(false);
     if (Ptr<CommandInputs> childInputs = _billboardingGroup->children())
     {
      _billboardingStyle = childInputs->addButtonRowCommandInput(_commandId + "_billboardingStyle", "style", false);
      if (_billboardingStyle) 
      {
       if (Ptr<ListItems> listItems = _billboardingStyle->listItems()) 
       {
        listItems->add("Screen", false, "./resources/One");
        listItems->add("Axis", false, "./resources/Two");
        listItems->add("Right Reading", false, "./resources/Three");
       }
      }
     }
    }
    //
   }
  }
 }
private:
 OnExecuteEventHandler onExecuteHandler_;
 OnDestroyEventHandler onDestroyHandler_;
 OnInputChangedEventHander onInputChangedHandler_;
} cmdCreated_;

extern "C" XI_EXPORT bool run(const char* context)
{
 _app = Application::get();
 if (!_app)
  return false;

 _ui = _app->userInterface();
 if (!_ui)
  return false;

 Ptr<Document> doc = _app->activeDocument();
 if (!doc)
  return false;

 Ptr<Products> prods = doc->products();
 if (!prods)
  return false;

 Ptr<Product> prod = prods->itemByProductType("DesignProductType");
 if (!prod)
  return false;

 _des = prod->cast<Design>();
 if (!_des)
  return false;

 // get the entry for custom graphics
 Ptr<Product> activeProd = _app->activeProduct();
 if (!activeProd)
  return false;

 Ptr<CAM> cam = activeProd->cast<CAM>();
 if (cam) {
  _cgGroups = cam->customGraphicsGroups();
 }
 else {
  Ptr<Component> rootComp = _des->rootComponent();
  if (!rootComp)
   return false;
  _cgGroups = rootComp->customGraphicsGroups();
 }
 if (!_cgGroups)
  return false;

 Ptr<CommandDefinition> command = createCommandDefinition();
 if (!command)
  return false;

 Ptr<CommandCreatedEvent> commandCreatedEvent = command->commandCreated();
 if (!commandCreatedEvent)
  return false;
 commandCreatedEvent->add(&cmdCreated_);
 command->execute();

 // prevent this module from being terminate when the script returns, because we are waiting for event handlers to fire
 adsk::autoTerminate(false);

 return true;
}

#ifdef XI_WIN

#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hmodule, DWORD reason, LPVOID reserved)
{
 switch (reason)
 {
 case DLL_PROCESS_ATTACH:
 case DLL_THREAD_ATTACH:
 case DLL_THREAD_DETACH:
 case DLL_PROCESS_DETACH:
  break;
 }
 return TRUE;
}

#endif // XI_WIN