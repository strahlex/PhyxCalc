#include "phyxcalculator.h"

PhyxCalculator::PhyxCalculator(QObject *parent) :
    QObject(parent)
{
    initialize();
}

void PhyxCalculator::initialize()
{
    //initialize variables
    m_expression = "";
    expressionIsParsable = false;
    valueBuffer = 1;
    prefixBuffer = "";
    unitBuffer = "";
    m_error = false;
    m_errorNumber = 0;
    m_errorPosition = 0;
    m_resultValue = 0;
    m_resultUnit = "";

    //initialize random number generator
    qsrand(QDateTime::currentMSecsSinceEpoch());

    // create earley parser
    earleyParser = new QEarleyParser();

    //map functions
    functionMap.insert("valueCheckComplex",     &PhyxCalculator::valueCheckComplex);
    functionMap.insert("valueCheckComplex2",    &PhyxCalculator::valueCheckComplex2);
    functionMap.insert("valueCheckPositive",         &PhyxCalculator::valueCheckPositive);
    functionMap.insert("valueCheckInteger",          &PhyxCalculator::valueCheckInteger);
    functionMap.insert("valueAdd",      &PhyxCalculator::valueAdd);
    functionMap.insert("valueSub",      &PhyxCalculator::valueSub);
    functionMap.insert("valueMul",      &PhyxCalculator::valueMul);
    functionMap.insert("valueDiv",      &PhyxCalculator::valueDiv);
    functionMap.insert("valueNeg",      &PhyxCalculator::valueNeg);
    functionMap.insert("valuePow",      &PhyxCalculator::valuePow);
    functionMap.insert("valuePow2",     &PhyxCalculator::valuePow2);
    functionMap.insert("valuePow3",     &PhyxCalculator::valuePow3);
    functionMap.insert("valueSin",      &PhyxCalculator::valueSin);
    functionMap.insert("valueArcsin",   &PhyxCalculator::valueArcsin);
    functionMap.insert("valueCos",      &PhyxCalculator::valueCos);
    functionMap.insert("valueArccos",   &PhyxCalculator::valueArccos);
    functionMap.insert("valueTan",      &PhyxCalculator::valueTan);
    functionMap.insert("valueArctan",   &PhyxCalculator::valueArctan);
    functionMap.insert("valueSinh",     &PhyxCalculator::valueSinh);
    functionMap.insert("valueArcsinh",  &PhyxCalculator::valueArcsinh);
    functionMap.insert("valueCosh",     &PhyxCalculator::valueCosh);
    functionMap.insert("valueArccosh",  &PhyxCalculator::valueArccosh);
    functionMap.insert("valueTanh",     &PhyxCalculator::valueTanh);
    functionMap.insert("valueArctanh",  &PhyxCalculator::valueArctanh);
    functionMap.insert("valueExp",      &PhyxCalculator::valueExp);
    functionMap.insert("valueLn",       &PhyxCalculator::valueLn);
    functionMap.insert("valueLog10",    &PhyxCalculator::valueLog10);
    functionMap.insert("valueLog2",     &PhyxCalculator::valueLog2);
    functionMap.insert("valueLogn",     &PhyxCalculator::valueLogn);
    functionMap.insert("valueRoot",     &PhyxCalculator::valueRoot);
    functionMap.insert("valueSqrt",     &PhyxCalculator::valueSqrt);
    functionMap.insert("valueAbs",      &PhyxCalculator::valueAbs);
    functionMap.insert("valueMax",      &PhyxCalculator::valueMax);
    functionMap.insert("valueMin",      &PhyxCalculator::valueMin);
    functionMap.insert("valuePi",       &PhyxCalculator::valuePi);
    functionMap.insert("valueInt",      &PhyxCalculator::valueInt);
    functionMap.insert("valueTrunc",    &PhyxCalculator::valueTrunc);
    functionMap.insert("valueFloor",    &PhyxCalculator::valueFloor);
    functionMap.insert("valueRound",    &PhyxCalculator::valueRound);
    functionMap.insert("valueCeil",     &PhyxCalculator::valueCeil);
    functionMap.insert("valueSign",     &PhyxCalculator::valueSign);
    functionMap.insert("valueHeaviside",&PhyxCalculator::valueHeaviside);
    functionMap.insert("valueRand",     &PhyxCalculator::valueRand);
    functionMap.insert("valueRandint",  &PhyxCalculator::valueRandint);
    functionMap.insert("valueRandg",    &PhyxCalculator::valueRandg);
    functionMap.insert("valueFaculty",  &PhyxCalculator::valueFaculty);
    functionMap.insert("complexArg",    &PhyxCalculator::complexArg);
    functionMap.insert("complexNorm",   &PhyxCalculator::complexNorm);
    functionMap.insert("complexConj",   &PhyxCalculator::complexConj);
    functionMap.insert("complexPolar",  &PhyxCalculator::complexPolar);

    functionMap.insert("unitCheckDimensionless",    &PhyxCalculator::unitCheckDimensionless);
    functionMap.insert("unitCheckDimensionless2",   &PhyxCalculator::unitCheckDimensionless2);
    functionMap.insert("unitCheckConvertible",      &PhyxCalculator::unitCheckConvertible);
    functionMap.insert("unitConvert",   &PhyxCalculator::unitConvert);
    functionMap.insert("unitMul",       &PhyxCalculator::unitMul);
    functionMap.insert("unitDiv",       &PhyxCalculator::unitDiv);
    functionMap.insert("unitPow",       &PhyxCalculator::unitPow);
    functionMap.insert("unitPow2",      &PhyxCalculator::unitPow2);
    functionMap.insert("unitPow3",      &PhyxCalculator::unitPow3);
    functionMap.insert("unitSqrt",      &PhyxCalculator::unitSqrt);
    functionMap.insert("unitRoot",      &PhyxCalculator::unitRoot);
    functionMap.insert("unitAdd",       &PhyxCalculator::unitAdd);
    functionMap.insert("unitRemove",    &PhyxCalculator::unitRemove);

    functionMap.insert("variableAdd",   &PhyxCalculator::variableAdd);
    functionMap.insert("variableRemove",&PhyxCalculator::variableRemove);
    functionMap.insert("variableLoad",  &PhyxCalculator::variableLoad);
    functionMap.insert("constantAdd",   &PhyxCalculator::constantAdd);
    functionMap.insert("constantRemove",&PhyxCalculator::constantRemove);
    functionMap.insert("constantLoad",  &PhyxCalculator::constantLoad);

    functionMap.insert("bufferUnit",    &PhyxCalculator::bufferUnit);
    functionMap.insert("bufferValue",   &PhyxCalculator::bufferValue);
    functionMap.insert("bufferPrefix",   &PhyxCalculator::bufferPrefix);
    functionMap.insert("bufferString",   &PhyxCalculator::bufferString);
    functionMap.insert("bufferUnitGroup",&PhyxCalculator::bufferUnitGroup);
    functionMap.insert("pushVariable",  &PhyxCalculator::pushVariable);

    functionMap.insert("outputVariable",&PhyxCalculator::outputVariable);
    functionMap.insert("unitGroupAdd",  &PhyxCalculator::unitGroupAdd);
    functionMap.insert("unitGroupRemove",&PhyxCalculator::unitGroupRemove);
    functionMap.insert("prefixAdd",     &PhyxCalculator::prefixAdd);
    functionMap.insert("prefixRemove",  &PhyxCalculator::prefixRemove);

    loadGrammar(":/settings/grammar");
    earleyParser->setStartSymbol("S");

    //initialize unit system
    unitSystem = new PhyxUnitSystem();
    connect(unitSystem, SIGNAL(unitAdded(QString)),
            this, SLOT(addUnitRule(QString)));
    connect(unitSystem, SIGNAL(unitRemoved(QString)),
            this, SLOT(removeUnitRule(QString)));
    connect(unitSystem, SIGNAL(prefixAdded(QString)),
            this, SLOT(addPrefixRule(QString)));
    connect(unitSystem, SIGNAL(prefixRemoved(QString)),
            this, SLOT(removePrefixRule(QString)));
    connect(unitSystem, SIGNAL(unitGroupAdded(QString)),
            this, SLOT(addUnitGroupRule(QString)));
    connect(unitSystem, SIGNAL(unitGroupRemoved(QString)),
            this, SLOT(removeUnitGroupRule(QString)));

    //initialize variable manager
    variableManager = new PhyxVariableManager();
    connect(variableManager, SIGNAL(variableAdded(QString)),
            this, SLOT(addVariableRule(QString)));
    connect(variableManager, SIGNAL(variableRemoved(QString)),
            this, SLOT(removeVariableRule(QString)));
    connect(variableManager, SIGNAL(constantAdded(QString)),
            this, SLOT(addConstantRule(QString)));
    connect(variableManager, SIGNAL(constantRemoved(QString)),
            this, SLOT(removeConstantRule(QString)));

    loadFile(":/settings/definitions");
}

void PhyxCalculator::loadGrammar(QString fileName)
{
    QFile file(fileName);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QStringList lines = QString::fromUtf8(file.readAll()).split('\n');
        foreach (QString line, lines)
        {
            if (line.trimmed().isEmpty() || (line.trimmed().at(0) == '#'))
                continue;

            if (line.contains("//"))
                line.truncate(line.indexOf("//"));

            QStringList ruleData = line.split(';');
            QString rule;
            QString functions;

            rule = ruleData.at(0).trimmed();
            if (ruleData.size() > 1)
                functions = ruleData.at(1).trimmed();

            addRule(rule, functions);
        }
    }
    else
        qFatal("Can't open file");
}

void PhyxCalculator::raiseException(int errorNumber)
{
    //qDebug() << exception;
    m_error = true;
    m_errorNumber = errorNumber;
    clearStack();
    emit outputError();
}

void PhyxCalculator::addRule(QString rule, QString functions)
{
    PhyxRule phyxRule;
    if (!functions.isEmpty())
        phyxRule.functions = functions.split(',');
    phyxRules.insert(rule, phyxRule);

    QStringList ruleFunctions;
    foreach (QString function, phyxRule.functions)
        ruleFunctions.append(function.trimmed());
    earleyParser->loadRule(rule, ruleFunctions);
}

void PhyxCalculator::addUnitRule(QString symbol)
{
    addRule(QString("unit=%1").arg(symbol), QString("bufferParameter, bufferUnit"));
}

void PhyxCalculator::removeUnitRule(QString symbol)
{
    earleyParser->removeRule(QString("unit=%1").arg(symbol));
}

void PhyxCalculator::addVariableRule(QString name)
{
    addRule(QString("variable=%1").arg(name), QString("bufferParameter, variableLoad"));
    emit variablesChanged();
}

void PhyxCalculator::removeVariableRule(QString name)
{
    earleyParser->removeRule(QString("variable=%1").arg(name));
    emit variablesChanged();
}

void PhyxCalculator::addConstantRule(QString name)
{
    addRule(QString("constant=%1").arg(name), QString("bufferParameter, constantLoad"));
    emit constantsChanged();
}

void PhyxCalculator::removeConstantRule(QString name)
{
    earleyParser->removeRule(QString("constant=%1").arg(name));
    emit constantsChanged();
}

void PhyxCalculator::addPrefixRule(QString symbol)
{
    addRule(QString("prefix=%1").arg(symbol), QString("bufferParameter, bufferPrefix"));
}

void PhyxCalculator::removePrefixRule(QString symbol)
{
    earleyParser->removeRule(QString("prefix=%1").arg(symbol));
}

void PhyxCalculator::addUnitGroupRule(QString name)
{
    addRule(QString("unitGroup=%1").arg(name), QString("bufferParameter, bufferUnitGroup"));
}

void PhyxCalculator::removeUnitGroupRule(QString name)
{
    earleyParser->removeRule(QString("unitGroup=%1").arg(name));
}

void PhyxCalculator::clearStack()
{
    foreach (PhyxVariable *variable, variableStack)
        delete variable;
    variableStack.clear();
}

void PhyxCalculator::clearResult()
{
    m_resultValue = PhyxValueDataType(0.0,0.0);
    m_resultUnit = "";
}

bool PhyxCalculator::setExpression(QString expression)
{
    if (expression.isEmpty())
    {
        earleyParser->clearWord();
        expressionIsParsable = false;
    }
    else if (!m_expression.isEmpty() && expression.indexOf(m_expression) == 0)      //new expression is old expression + string
    {
        QString string = expression.mid(m_expression.size());
        foreach (QChar character, string)
        {
            expressionIsParsable = earleyParser->addSymbol(character);
        }
    }
    else if (m_expression.indexOf(expression) == 0) //new expression is old expression - string
    {
        int  count = m_expression.size() - expression.size();
        for (int i = 0; i < count; i++)
        {
            expressionIsParsable = earleyParser->removeSymbol();
        }
    }
    else
    {
        expressionIsParsable = earleyParser->parseWord(expression);
    }

    m_expression = expression;
    return expressionIsParsable;
}

bool PhyxCalculator::evaluate()
{
    if (expressionIsParsable)
    {
        clearResult();
        m_error = false;

        QList<EarleyTreeItem> earleyTree;
        earleyTree = earleyParser->getTree();

        for (int i = (earleyTree.size()-1); i >= 0; i--)
        {
            EarleyTreeItem *earleyTreeItem = &earleyTree[i];

            m_errorPosition = earleyTreeItem->startPos;     //just in case
            //PhyxRule phyxRule = phyxRules.value(earleyTreeItem->rule);

            //if (!phyxRule.functions.isEmpty())
            if (!earleyTreeItem->rule->functions.isEmpty())
            {
                foreach (QString function, earleyTreeItem->rule->functions)
                {
                    if (function == "bufferParameter")
                        parameterBuffer = m_expression.mid(earleyTreeItem->startPos, earleyTreeItem->endPos - earleyTreeItem->startPos + 1);
                    else
                    {
                        if (functionMap.value(function, NULL) != NULL)
                            (this->*functionMap.value(function))();
                        else
                            qFatal(tr("Function %1 not found!").arg(function).toLocal8Bit());

                        if (this->hasError())
                            return false;
                    }
                }
            }
        }
        return true;
    }
    else
    {
        raiseException(SyntaxError);
        return false;
    }
}

void PhyxCalculator::loadFile(QString fileName)
{
    QFile file(fileName);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QStringList lines = QString::fromUtf8(file.readAll()).split('\n');
        foreach (QString line, lines)
        {
            if (line.contains("//"))
                line.truncate(line.indexOf("//"));

            if (line.trimmed().isEmpty() || (line.trimmed().at(0) == '#'))
                continue;

            this->setExpression(line.trimmed());
            if (!this->evaluate())
                qDebug() << line;
        }
    }
}

void PhyxCalculator::clearVariables()
{
    variableManager->clearVariables();
}

PhyxVariableManager::PhyxVariableMap *PhyxCalculator::variables() const
{
    return variableManager->variables();
}

PhyxVariableManager::PhyxVariableMap *PhyxCalculator::constants() const
{
    return variableManager->constants();
}

QString PhyxCalculator::errorString() const
{
    QString output;

    switch (m_errorNumber)
    {
    case SyntaxError:       output.append(tr("Syntax Error!"));
                            break;
    case ValueComplexError: output.append(tr("Value is complex"));
                            break;
    case ValueNotPositiveError: output.append(tr("Value is negative"));
                            break;
    case ValueNotIntegerError: output.append(tr("Only integer values"));
                            break;
    case UnitNotDimensionlessError: output.append(tr("Unit is not dimensionless"));
                            break;
    case UnitsNotConvertibleError: output.append(tr("Units not convertible"));
                            break;
    case PrefixError: output.append(tr("Prefix does not fit unit"));
                            break;
    }

    return tr("error at position %1: %2").arg(m_errorPosition).arg(output);
}

QString PhyxCalculator::complexToString(const PhyxValueDataType number)
{
    QString string;
    boost::format format("%.10g");
    int components = 0;

    if (number.real() != 0)
    {
        std::stringstream ss;
        ss << format % number.real();
        string.append(QString::fromStdString(ss.str()));
        components++;
    }

    if (number.imag() != 0)
    {
        if (!string.isEmpty())
            string.append("+");
        if ((number.imag() != 1) && (number.imag() != -1))
        {
            std::stringstream ss;
            ss << format % number.imag();
            string.append(QString::fromStdString(ss.str()));
        }
        else if (number.imag() == -1)
        {
            string.append("-");
        }
        string.append("i");
        components++;
    }

    string.replace("+-","-");

    if (string.isEmpty())
        string.append("0");
    else if (components == 2)
    {
        string.prepend("(");
        string.append(")");
    }

    return string;
}

PhyxValueDataType PhyxCalculator::stringToComplex(QString string)
{
    PhyxValueDataType value;

    if (string.indexOf(QRegExp("[ij]")) != -1)
    {
        string.remove(QRegExp("[ij]"));
        if (string.isEmpty())
            string="1";

        long double tmpValue;
        std::istringstream inStream(string.toStdString());
        inStream >> tmpValue;
        value = PhyxValueDataType(0.0,tmpValue);
    }
    else
    {
        long double tmpValue;
        std::istringstream inStream(string.toStdString());
        inStream >> tmpValue;
        value = PhyxValueDataType(tmpValue,0.0);
    }

    return value;
}

void PhyxCalculator::valueCheckComplex()
{
    PhyxVariable *variable1 = variableStack.pop();

    if (variable1->value().imag() != 0)
        raiseException(ValueComplexError);

    variableStack.push(variable1);
}

void PhyxCalculator::valueCheckComplex2()
{
    PhyxVariable *variable1 = variableStack.pop();
    PhyxVariable *variable2 = variableStack.pop();

    if ((variable1->value().imag() != 0) || (variable2->value().imag() != 0))
        raiseException(ValueComplexError);

    variableStack.push(variable2);
    variableStack.push(variable1);
}

void PhyxCalculator::valueCheckPositive()
{
    PhyxVariable *variable1 = variableStack.pop();

    if (variable1->value().real() < 0)
        raiseException(ValueNotPositiveError);

    variableStack.push(variable1);
}

void PhyxCalculator::valueCheckInteger()
{
    PhyxVariable *variable1 = variableStack.pop();

    if ((int)variable1->value().real() != variable1->value().real())
        raiseException(ValueNotIntegerError);

    variableStack.push(variable1);
}

void PhyxCalculator::valueAdd()
{
    PhyxVariable *variable2 = variableStack.pop();
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(variable1->value() + variable2->value());
    variableStack.push(variable1);

    delete variable2;
}

void PhyxCalculator::valueSub()
{
    PhyxVariable *variable2 = variableStack.pop();
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(variable1->value() - variable2->value());
    variableStack.push(variable1);

    delete variable2;
}

void PhyxCalculator::valueMul()
{
    PhyxVariable *variable2 = variableStack.pop();
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(variable1->value() * variable2->value());
    variableStack.push(variable1);

    delete variable2;
}

void PhyxCalculator::valueDiv()
{
    PhyxVariable *variable2 = variableStack.pop();
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(variable1->value() / variable2->value());
    variableStack.push(variable1);

    delete variable2;
}

void PhyxCalculator::valueNeg()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(-variable1->value());
    variableStack.push(variable1);
}

void PhyxCalculator::valuePow()
{
    PhyxVariable *variable2 = variableStack.pop();
    PhyxVariable *variable1 = variableStack.pop();

    if (variable2->value().imag() == 0)     // precision fix
        variable1->setValue(pow(variable1->value(),(double)variable2->value().real()));
    else
        variable1->setValue(pow(variable1->value(),variable2->value()));
    variableStack.push(variable1);

    delete variable2;
}

void PhyxCalculator::valuePow2()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(pow(variable1->value(),2));
    variableStack.push(variable1);
}

void PhyxCalculator::valuePow3()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(pow(variable1->value(),3));
    variableStack.push(variable1);
}

void PhyxCalculator::valueSin()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(sin(variable1->value()));
    variableStack.push(variable1);
}

void PhyxCalculator::valueArcsin()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(boost::math::asin(variable1->value()));
    variableStack.push(variable1);
}

void PhyxCalculator::valueCos()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(cos(variable1->value()));
    variableStack.push(variable1);
}

void PhyxCalculator::valueArccos()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(boost::math::acos(variable1->value()));
    variableStack.push(variable1);
}

void PhyxCalculator::valueTan()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(tan(variable1->value()));
    variableStack.push(variable1);
}

void PhyxCalculator::valueArctan()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(boost::math::atan(variable1->value()));
    variableStack.push(variable1);
}

void PhyxCalculator::valueSinh()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(sinh(variable1->value()));
    variableStack.push(variable1);
}

void PhyxCalculator::valueArcsinh()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(boost::math::asinh(variable1->value()));
    variableStack.push(variable1);
}

void PhyxCalculator::valueCosh()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(cosh(variable1->value()));
    variableStack.push(variable1);
}

void PhyxCalculator::valueArccosh()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(boost::math::acosh(variable1->value()));
    variableStack.push(variable1);
}

void PhyxCalculator::valueTanh()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(tanh(variable1->value()));
    variableStack.push(variable1);
}

void PhyxCalculator::valueArctanh()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(boost::math::atanh(variable1->value()));
    variableStack.push(variable1);
}

void PhyxCalculator::valueExp()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(exp(variable1->value()));
    variableStack.push(variable1);
}

void PhyxCalculator::valueLn()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(log(variable1->value()));
    variableStack.push(variable1);
}

void PhyxCalculator::valueLog10()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(log10(variable1->value()));
    variableStack.push(variable1);
}

void PhyxCalculator::valueLog2()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(log(variable1->value()));
    variableStack.push(variable1);
}

void PhyxCalculator::valueLogn()
{
    PhyxVariable *variable2 = variableStack.pop();
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(log(variable2->value()) / log(variable1->value()));
    variableStack.push(variable1);

    delete variable2;
}

void PhyxCalculator::valueRoot()
{
    PhyxVariable *variable2 = variableStack.pop();
    PhyxVariable *variable1 = variableStack.pop();

    PhyxValueDataType one(1,0);
    variable1->setValue(pow(variable2->value(), one/variable1->value()));

    variableStack.push(variable1);

    delete variable2;
}

void PhyxCalculator::valueSqrt()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(sqrt(variable1->value()));
    variableStack.push(variable1);
}

void PhyxCalculator::valueAbs()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(abs(variable1->value()));
    variableStack.push(variable1);
}

void PhyxCalculator::valueMax()
{
    PhyxVariable *variable1 = variableStack.pop();
    PhyxVariable *variable2 = variableStack.pop();

    if (variable1->value().real() >= variable2->value().real())
    {
        variableStack.push(variable1);
        delete variable2;
    }
    else
    {
        variableStack.push(variable2);
        delete variable1;
    }
}

void PhyxCalculator::valueMin()
{
    PhyxVariable *variable1 = variableStack.pop();
    PhyxVariable *variable2 = variableStack.pop();

    if (variable1->value().real()<= variable2->value().real())
    {
        variableStack.push(variable1);
        delete variable2;
    }
    else
    {
        variableStack.push(variable2);
        delete variable1;
    }

    delete variable2;
}

void PhyxCalculator::valuePi()
{
    valueBuffer = M_PI;
    unitBuffer = "";
    pushVariable();
}

void PhyxCalculator::valueInt()
{
    PhyxVariable *variable1 = variableStack.pop();

    PhyxValueDataType value((int)variable1->value().real(),0);
    variable1->setValue(value);
    variableStack.push(variable1);
}

void PhyxCalculator::valueTrunc()
{
    PhyxVariable *variable1 = variableStack.pop();

    PhyxValueDataType value(boost::math::trunc(variable1->value().real()),0);
    variable1->setValue(value);
    variableStack.push(variable1);
}

void PhyxCalculator::valueFloor()
{
    PhyxVariable *variable1 = variableStack.pop();

    PhyxValueDataType value(floor(variable1->value().real()),0);
    variable1->setValue(value);
    variableStack.push(variable1);
}

void PhyxCalculator::valueRound()
{
    PhyxVariable *variable1 = variableStack.pop();

    PhyxValueDataType value(boost::math::round(variable1->value().real()),0);
    variable1->setValue(value);
    variableStack.push(variable1);
}

void PhyxCalculator::valueCeil()
{
    PhyxVariable *variable1 = variableStack.pop();

    PhyxValueDataType value(ceil(variable1->value().real()),0);
    variable1->setValue(value);
    variableStack.push(variable1);
}

void PhyxCalculator::valueSign()
{
    PhyxVariable *variable1 = variableStack.pop();

    PhyxValueDataType value(boost::math::copysign((long double)1.0,variable1->value().real()),0);
    variable1->setValue(value);
    variableStack.push(variable1);
}

void PhyxCalculator::valueHeaviside()
{
    PhyxVariable *variable1 = variableStack.pop();

    if (variable1->value().real() >= 0)
        variable1->setValue(PhyxValueDataType(1,0));
    else
        variable1->setValue(PhyxValueDataType(0,0));
    variableStack.push(variable1);
}

void PhyxCalculator::valueRand()
{
    valueBuffer = qrand();
    unitBuffer = "";
    pushVariable();
}

void PhyxCalculator::valueRandint()
{
    valueBuffer = (int)qrand();
    unitBuffer = "";
    pushVariable();
}

void PhyxCalculator::valueRandg()
{
    PhyxVariable *variable1 = variableStack.pop();
    PhyxVariable *variable2 = variableStack.pop();

    int min, max;
    min = variable1->value().real();
    max = variable2->value().real();
    variable1->setValue(PhyxValueDataType(min + qrand()%(max-min+1),0));
    variableStack.push(variable1);

    delete variable2;
}

void PhyxCalculator::valueFaculty()
{
    PhyxVariable *variable1 = variableStack.pop();

    long double value = variable1->value().real();

    if (value < 0)
    {
        raiseException(ValueNotPositiveError);
        return;
    }
    else if ((int)value != value)
    {
        raiseException(ValueNotIntegerError);
        return;
    }

    int n = (int)boost::math::round(value);
    value = 1;
    for (int i = 2; i <=n; i++)
        value *= i;

    variable1->setValue(PhyxValueDataType(value,0));
    variableStack.push(variable1);
}

void PhyxCalculator::complexArg()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(arg(variable1->value()));
    variableStack.push(variable1);
}

void PhyxCalculator::complexNorm()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(norm(variable1->value()));
    variableStack.push(variable1);
}

void PhyxCalculator::complexConj()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->setValue(conj(variable1->value()));
    variableStack.push(variable1);
}

void PhyxCalculator::complexPolar()
{
    PhyxVariable *variable1 = variableStack.pop();
    variable1->setValue(std::polar(abs(variable1->value()), arg(variable1->value())));
    variableStack.push(variable1);
}

void PhyxCalculator::unitCheckDimensionless()
{
    PhyxVariable *variable1 = variableStack.pop();

    if (!variable1->unit()->isDimensionlessUnit())
        raiseException(UnitNotDimensionlessError);

    variableStack.push(variable1);
}

void PhyxCalculator::unitCheckDimensionless2()
{
    PhyxVariable *variable1 = variableStack.pop();
    PhyxVariable *variable2 = variableStack.pop();

    if (!variable1->unit()->isDimensionlessUnit() || !variable2->unit()->isDimensionlessUnit())
        raiseException(UnitNotDimensionlessError);

    variableStack.push(variable2);
    variableStack.push(variable1);
}

void PhyxCalculator::unitCheckConvertible()
{
    PhyxVariable *variable2 = variableStack.pop();
    PhyxVariable *variable1 = variableStack.pop();

    if (!variable1->unit()->isConvertible(variable2->unit()))
        raiseException(UnitsNotConvertibleError);
    else
        variable1->unit()->compoundsEqualize(variable2->unit());

    variableStack.push(variable1);
    variableStack.push(variable2);
}

void PhyxCalculator::unitConvert()
{
    PhyxVariable *variable2 = variableStack.pop();
    PhyxVariable *variable1 = variableStack.pop();

    variable1->convertUnit(variable2->unit());
    variableStack.push(variable1);

    delete variable2;
}

void PhyxCalculator::unitMul()
{
    PhyxVariable *variable2 = variableStack.pop();
    PhyxVariable *variable1 = variableStack.pop();

    variable1->unit()->multiply(variable2->unit());
    variableStack.push(variable1);
    variableStack.push(variable2);
}

void PhyxCalculator::unitDiv()
{
    PhyxVariable *variable2 = variableStack.pop();
    PhyxVariable *variable1 = variableStack.pop();

    variable1->unit()->divide(variable2->unit());
    variableStack.push(variable1);
    variableStack.push(variable2);
}

void PhyxCalculator::unitPow()
{
    PhyxVariable *variable2 = variableStack.pop();
    PhyxVariable *variable1 = variableStack.pop();

    variable1->unit()->raise(variable2->value().real());
    variableStack.push(variable1);
    variableStack.push(variable2);
}

void PhyxCalculator::unitPow2()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->unit()->raise(2);
    variableStack.push(variable1);
}

void PhyxCalculator::unitPow3()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->unit()->raise(3);
    variableStack.push(variable1);
}

void PhyxCalculator::unitRoot()
{
    PhyxVariable *variable2 = variableStack.pop();
    PhyxVariable *variable1 = variableStack.pop();

    variable1->unit()->root(variable2->value().real());
    variableStack.push(variable1);
    variableStack.push(variable2);
}

void PhyxCalculator::unitSqrt()
{
    PhyxVariable *variable1 = variableStack.pop();

    variable1->unit()->root(2);
    variableStack.push(variable1);
}

void PhyxCalculator::unitAdd()
{
    if (variableStack.isEmpty())        // <- base unit
    {
        unitSystem->addBaseUnit(stringBuffer, 0, unitGroupBuffer, prefixBuffer);
    }
    else
    {
        PhyxVariable *variable1 = NULL;
        PhyxVariable *variable2 = NULL;
        if (!variableStack.size() == 2)
        {
            variable2 = variableStack.pop();
            variable1 = variableStack.pop();
        }
        else
            variable1 = variableStack.pop();

        variable1->unit()->simplify();

        PhyxUnit *unit = new PhyxUnit();
        unit->setPowers(variable1->unit()->powers());
        unit->setUnitGroup(unitGroupBuffer);
        unit->setPreferedPrefix(prefixBuffer);
        unit->setSymbol(stringBuffer);
        unit->setFlags(0);
        unit->setScaleFactor(variable1->value().real());
        if (variable2 != NULL)
            unit->setOffset(variable2->value().real());

        unitSystem->addDerivedUnit(unit);

        delete variable1;
    }

    stringBuffer.clear();
    unitGroupBuffer.clear();
    prefixBuffer.clear();
}

void PhyxCalculator::unitRemove()
{

}

void PhyxCalculator::variableAdd()
{
    PhyxVariable *variable1 = variableStack.pop();

    variableManager->addVariable(stringBuffer, variable1);
    stringBuffer.clear();
}

void PhyxCalculator::variableRemove()
{
    variableManager->removeVariable(parameterBuffer);
}

void PhyxCalculator::variableLoad()
{
    variableStack.push(variableManager->getVariable(parameterBuffer));
}

void PhyxCalculator::constantAdd()
{
    PhyxVariable *variable1 = variableStack.pop();

    variableManager->addConstant(stringBuffer, variable1);
    stringBuffer.clear();
}

void PhyxCalculator::constantRemove()
{
    variableManager->removeConstant(parameterBuffer);
}

void PhyxCalculator::constantLoad()
{
    variableStack.push(variableManager->getConstant(parameterBuffer));
}

void PhyxCalculator::bufferUnit()
{
    //get the unit
    unitBuffer = parameterBuffer;
}

void PhyxCalculator::bufferValue()
{
    valueBuffer = stringToComplex(parameterBuffer);
}

void PhyxCalculator::bufferPrefix()
{
    prefixBuffer = parameterBuffer;
}

void PhyxCalculator::bufferString()
{
    stringBuffer = parameterBuffer;
    stringBuffer.remove('"');

    if (stringBuffer.at(0).isNumber())
        raiseException(StringNotValidError);
}

void PhyxCalculator::bufferUnitGroup()
{
    unitGroupBuffer = parameterBuffer;
}

void PhyxCalculator::pushVariable()
{
    //create new variable
    PhyxVariable *variable = new PhyxVariable();
    variable->unit()->setUnitSystem(unitSystem);
    if (!unitBuffer.isEmpty())
        variable->setUnit(unitSystem->unit(unitBuffer));
    if (!prefixBuffer.isEmpty())
    {
        PhyxUnitSystem::PhyxPrefix prefix = unitSystem->prefix(prefixBuffer, variable->unit()->unitGroup());
        if (!prefix.unitGroup.isEmpty())
        {
            valueBuffer *= PhyxValueDataType(prefix.value, 0.0);
            variable->setValue(valueBuffer);
        }
        else
        {
            raiseException(PrefixError);
        }
    }
    else
        variable->setValue(valueBuffer);

    //push it to the stack
    variableStack.push(variable);

    //cleanup
    valueBuffer = 1;
    unitBuffer.clear();
    prefixBuffer.clear();
}

void PhyxCalculator::outputVariable()
{
    if (!variableStack.isEmpty())
    {
        PhyxVariable *variable1 = variableStack.pop();
        m_resultValue = variable1->value();
        m_resultUnit = variable1->unit()->symbol();
        delete variable1;

        emit outputResult();
    }
}

void PhyxCalculator::unitGroupAdd()
{
    unitSystem->addUnitGroup(stringBuffer);
    stringBuffer.clear();
}

void PhyxCalculator::unitGroupRemove()
{
    unitSystem->removeUnitGroup(unitGroupBuffer);
    unitGroupBuffer.clear();
}

void PhyxCalculator::prefixAdd()
{
    PhyxVariable *variable1 = variableStack.pop();
    unitSystem->addPrefix(stringBuffer, variable1->value().real(), unitGroupBuffer);

    unitGroupBuffer.clear();
    stringBuffer.clear();
    delete variable1;
}

void PhyxCalculator::prefixRemove()
{
    unitSystem->removePrefix(prefixBuffer);
    prefixBuffer.clear();
}
