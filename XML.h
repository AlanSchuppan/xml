#pragma once

#include <string>
#include <vector>

using namespace std;

//##############################################################################
// CValue
//##############################################################################
//##############################################################################

class CValue {
  static const char sReservedChars[];
  static const char * const sReservedRefs[];
  static const uint32_t sNRefs;
protected:
  string mName;
  string mValue;

  static string EntityRefIns(const string &text);
  static string EntityRefRem(const string &text);
public:
  CValue(const string &name = "", const string &value = "");
  CValue(const string &name, int32_t value);
  CValue(const string &name, uint32_t value);
  CValue(const string &name, double value);
  CValue(const CValue &other);
  CValue(CValue &&other);
  CValue &operator=(const CValue &other);
  CValue &operator=(CValue &&other);

  string Name() const;
  void   Name(const string &value);
  string Value() const;
  void   Value(const string &value);

  void Value(int32_t value);
  void Value(int32_t *pvalue);
  void Value(uint32_t value);
  void Value(uint32_t *pvalue);
  void Value(double value);
  void Value(double *pvalue);
};

//##############################################################################
// CNode
//##############################################################################
//##############################################################################

enum class ETagType { Unknown, Open, Close, Complete };

class CNode : public CValue {
  vector<CValue> mAttributes;
  vector<CNode> mChildren;

  static bool NextTag(istream &inStream, string &name, vector<CValue> &attributes);
  void PathParse(const string &path, vector<string> &parsing);
public:
  CNode(const string &name, const string &value = "");
  CNode(const CNode &other);
  CNode(CNode &&other);
  CNode &operator=(const CNode &other);
  CNode &operator=(CNode &&other);

  void AddributeAdd(const CValue &value);
  void AddributeAdd(const string &name, const string &value);
  void ChildAdd(const CNode &node);
  void ChildAdd(CNode &&node);
  CNode *Path(const string &path, size_t ix = 0);
  void Print(ostream &outStream, int level = 0) const;
};

//##############################################################################
