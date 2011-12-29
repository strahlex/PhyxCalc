#include "phyxcalculator.h"

PhyxCalculator::PhyxCalculator(QObject *parent) :
    QObject(parent)
{
    initialize();

    earleyParser.setStartSymbol("S");
}

void PhyxCalculator::initialize()
{
    //initialize variables
    expression = "";
    expressionIsParsable = false;

    //initialize random number generator
    qsrand(QDateTime::currentMSecsSinceEpoch());

    //map functions
    functionMap.insert("valueAdd",      &PhyxCalculator::valueAdd);
    functionMap.insert("valueSub",      &PhyxCalculator::valueSub);
    functionMap.insert("valueMul",      &PhyxCalculator::valueMul);
    functionMap.insert("valueDiv",      &PhyxCalculator::valueDiv);
    functionMap.insert("valueNeg",      &PhyxCalculator::valueNeg);
    functionMap.insert("valueExponent", &PhyxCalculator::valueExponent);
    functionMap.insert("valueExp2",     &PhyxCalculator::valueExp2);
    functionMap.insert("valueExp3",     &PhyxCalculator::valueExp3);
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

    functionMap.insert("unit0",         &PhyxCalculator::unit0);
    functionMap.insert("unitCheckEqual",&PhyxCalculator::unitCheckEqual);
    functionMap.insert("unitCheck0",    &PhyxCalculator::unitCheck0);
    functionMap.insert("unitCheck0k",   &PhyxCalculator::unitCheck0k);
    functionMap.insert("unitMul",       &PhyxCalculator::unitMul);
    functionMap.insert("unitDiv",       &PhyxCalculator::unitDiv);
    functionMap.insert("unitExponent",  &PhyxCalculator::unitExponent);
    functionMap.insert("unitExp2",      &PhyxCalculator::unitExp2);
    functionMap.insert("unitExp3",      &PhyxCalculator::unitExp3);
    functionMap.insert("unitSqrt",      &PhyxCalculator::unitSqrt);

    functionMap.insert("numberPush",    &PhyxCalculator::numberPush);
    functionMap.insert("variableAdd",   &PhyxCalculator::variableAdd);
    functionMap.insert("variableRemove",&PhyxCalculator::variableRemove);

    paramFunctionMap.insert("numberBuf",        &PhyxCalculator::numberBuf);
    paramFunctionMap.insert("variablePush",      &PhyxCalculator::variablePush);

    loadGrammar(":/settings/grammar");
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

            QStringList ruleData = line.split(';');
            QString rule;
            QString functions;
            QString paramFunction;
            QString parameter;

            rule = ruleData.at(0).trimmed();
            if (ruleData.size() > 1)
                functions = ruleData.at(1).trimmed();
            if (ruleData.size() > 2)
                paramFunction = ruleData.at(2).trimmed();
            if (ruleData.size() > 3)
                parameter = ruleData.at(3).trimmed();

            addRule(rule, functions, paramFunction, parameter);
        }
    }
    else
        qFatal("Can't open file");
}

void PhyxCalculator::raiseException(QString exception)
{
    qDebug() << exception;
}

void PhyxCalculator::addRule(QString rule, QString functions, QString paramFunction, QString parameter)
{
    PhyxRule phyxRule;
    if (!functions.isEmpty())
        phyxRule.functions = functions.split(',');
    phyxRule.paramFunction = paramFunction;
    phyxRule.parameter = parameter;
    phyxRules.insert(rule, phyxRule);

    earleyParser.loadRule(rule);
}

bool PhyxCalculator::setExpression(QString m_expression)
{
    if (m_expression.isEmpty())
    {
        earleyParser.clearWord();
        expressionIsParsable = false;
    }
    else if (!expression.isEmpty() && m_expression.indexOf(expression) == 0)      //new expression is old expression + string
    {
        QString string = m_expression.mid(expression.size());
        foreach (QChar character, string)
        {
            expressionIsParsable = earleyParser.addSymbol(character);
        }
    }
    else if (expression.indexOf(m_expression) == 0) //new expression is old expression - string
    {
        int  count = expression.size() - m_expression.size();
        for (int i = 0; i < count; i++)
        {
            expressionIsParsable = earleyParser.removeSymbol();
        }
    }
    else
    {
        expressionIsParsable = earleyParser.parseWord(m_expression);
    }

    expression = m_expression;
    return expressionIsParsable;
}

bool PhyxCalculator::evaluate()
{
    if (expressionIsParsable)
    {
        QList<EarleyTreeItem> earleyTree;
        earleyTree = earleyParser.getTree();

        for (int i = (earleyTree.size()-1); i > 0; i--)
        {
            EarleyTreeItem *earleyTreeItem = &earleyTree[i];
            PhyxRule phyxRule = phyxRules.value(earleyTreeItem->rule);

            if (!phyxRule.functions.isEmpty())
            {
                foreach (QString function, phyxRule.functions)
                {
                    (this->*functionMap.value(function))();
                }
            }
            if (!phyxRule.paramFunction.isEmpty())
                (this->*paramFunctionMap.value(phyxRule.paramFunction))(phyxRule.parameter);
        }
        qDebug() << (double)valueStack.pop();
        return true;
    }
    else
    {
        raiseException("Syntax Error!");
        return false;
    }
}

void PhyxCalculator::valueFaculty()
{
    PhyxValueDataType value = valueStack.pop();

    if (value < 0)
        raiseException("Only positive values");
    if ((int)value != value)
        raiseException("Only integer values");

    int n = (int)round(value);
    value = 1;
    for (int i = 2; i <=n; i++)
        value *= i;

    valueStack.push(value);
}

void PhyxCalculator::variableAdd()
{
    PhysicalVariable variable;
    variable.value = valueStack.pop();
    variable.unit = unitStack.pop();
    variableMap.insert(stringBuffer, variable);
    stringBuffer.clear();
}

void PhyxCalculator::variableRemove()
{
    variableMap.remove(stringBuffer);
    stringBuffer.clear();
}

void PhyxCalculator::variablePush(QString name)
{
    PhysicalVariable variable = variableMap.value(name);
    valueStack.push(variable.value);
    valueStack.push(variable.unit);
}