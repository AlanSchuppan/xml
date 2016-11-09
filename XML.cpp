#include "stdafx.h"

#include <iostream>

#include "XML.h"

using namespace std;

//##############################################################################
// CValue
//##############################################################################
//##############################################################################

// Characters in sReservedChars must map to strings in sReservedRefs one-to-one.
const char CValue::sReservedChars[] = R"(<>&'")";
const uint32_t CValue::sNRefs = sizeof(CValue::sReservedChars) - 0;
const char * const CValue::sReservedRefs[CValue::sNRefs] = {
  "&lt;", "&gt;", "&amp;", "&apos;", "&quot;"
};

//------------------------------------------------------------------------------
// string CValue::EntityRefIns(const string &text)
//
//
//
string CValue::EntityRefIns(const string &text) {
  static string ReservedChars;
  string Result(text);
  if (ReservedChars.empty())
    ReservedChars = sReservedChars;
  size_t Pos = (!Result.empty())
    ? Result.find_last_of(sReservedChars, text.size() - 1) : string::npos;
  while (Pos != string::npos) {
    string sTargetChars(sReservedChars);
    size_t Ix = ReservedChars.find(Result[Pos]);
    Result.replace(Pos, 1, sReservedRefs[Ix]);
    if (Pos-- == 0)
      break;
    Pos = Result.find_last_of(sReservedChars, Pos);
  }
  return Result;
}

//------------------------------------------------------------------------------
// string CValue::EntityRefRem(const string &text)
//
//
//
string CValue::EntityRefRem(const string &text) {
  //string Result(text);
  string Result{};
  size_t Pos = Result.size();
  Pos = (Pos > 0) ? Result.rfind('&', Pos - 1) : string::npos;
  while (Pos != string::npos) {
    size_t End = Result.find(';', Pos);
    if (End != string::npos) {
      string Target(Result.substr(Pos, End - Pos + 1));
      for (size_t Ix = 0; Ix < sNRefs; ++Ix) {
        if (Target == CValue::sReservedRefs[Ix]) {
          Result.replace(Pos, End - Pos + 1, 1, sReservedChars[Ix]);
          break;
        }
      }
    }
    Pos = (Pos > 0) ? Result.rfind('&', Pos - 1) : string::npos;
  }

  return Result;
}

//------------------------------------------------------------------------------
// CValue::CValue(const string &name, const string &value)
//
//
//
CValue::CValue(const string &name, const string &value) :
  mName(name), mValue(value) {
}

//CValue::CValue(const string &name, uint32_t value) :
//  mName(name), mValue(to_string(value)) {
//}

//------------------------------------------------------------------------------
// CValue::CValue(const string &name, uint32_t value)
//
//
//
CValue::CValue(const string &name, uint32_t value) :
  CValue(name, to_string(value)) {
}

//------------------------------------------------------------------------------
// CValue::CValue(const string &name, int32_t value)
//
//
//
CValue::CValue(const string &name, int32_t value) :
  CValue(name, to_string(value)) {
}

//------------------------------------------------------------------------------
// CValue::CValue(const string &name, double value)
//
//
//
CValue::CValue(const string &name, double value) :
  CValue(name, to_string(value)) {
}




//------------------------------------------------------------------------------
// void CValue::Value(int32_t value)
//
//
//
void CValue::Value(int32_t value) {
  mValue = to_string(value);
}

//------------------------------------------------------------------------------
// void CValue::Value(int32_t *pvalue)
//
//
//
void CValue::Value(int32_t *pvalue) {
  *pvalue = stol(mValue);
}

//------------------------------------------------------------------------------
// void CValue::Value(uint32_t value)
//
//
//
void CValue::Value(uint32_t value) {
  mValue = to_string(value);
}

//------------------------------------------------------------------------------
// void CValue::Value(uint32_t *pvalue)
//
//
//
void CValue::Value(uint32_t *pvalue) {
  *pvalue = stoul(mValue);
}

//------------------------------------------------------------------------------
// void CValue::Value(double value)
//
//
//
void CValue::Value(double value) {
  mValue = to_string(value);
}

//------------------------------------------------------------------------------
// void CValue::Value(double *pvalue) 
//
//
//
void CValue::Value(double *pvalue) {
  *pvalue = stod(mValue);
}




//------------------------------------------------------------------------------
// CValue::CValue(const CValue &other)
//
//
//
CValue::CValue(const CValue &other) : mName(other.mName), mValue(other.mValue) {
}

//------------------------------------------------------------------------------
// CValue::CValue(CValue &&other)
//
//
//
CValue::CValue(CValue &&other) : mName(other.mName), mValue(other.mValue) {
}

//------------------------------------------------------------------------------
// CValue &CValue::operator=(const CValue &other)
//
//
//
CValue &CValue::operator=(const CValue &other) {
  mName = other.mName;
  mValue = other.mValue;
  return *this;
}

//------------------------------------------------------------------------------
// CValue &CValue::operator=(CValue &&other)
//
//
//
CValue &CValue::operator=(CValue &&other) {
  mName = move(other.mName);
  mValue = move(other.mValue);
  return *this;
}

//------------------------------------------------------------------------------
// string CValue::Name() const
//
//
//
string CValue::Name() const {
  return mName;
}

//------------------------------------------------------------------------------
// void CValue::Name(const string &name)
//
//
//
void CValue::Name(const string &name) {
  mName = name;
}

//------------------------------------------------------------------------------
// string CValue::Value() const
//
//
//
string CValue::Value() const {
  return mValue;
}

//------------------------------------------------------------------------------
// void CValue::Value(const string &value)
//
//
//
void CValue::Value(const string &value) {
  mValue = value;
}

//##############################################################################
// CNode
//##############################################################################
//##############################################################################

//------------------------------------------------------------------------------
// ETagType CNode::NextTag(istream &inStream, string &name, vector<CValue> &attributes)
//
//
//
ETagType CNode::NextTag(istream &inStream, string &name, vector<CValue> &attributes) {
  name.clear();
  attributes.clear();

  ETagType TagType = ETagType::Unknown;
  char Ch;
  inStream >> noskipws >> Ch;
  while (Ch != '<')
    inStream >> Ch;
  string Tag;
  inStream >> Ch;
  while (Ch != '>') {
    Tag += Ch;
    inStream >> Ch;
  }

  if (!Tag.empty() && Tag[0] == '/') {
    name = Tag.substr(1, Tag.size() - 1);
    return ETagType::Close;
  }

  size_t Pos = 0;
  bool First = true;
  string Targets(R"( /)");
  while (Pos < Tag.size()) {
    size_t End = Tag.find_first_of(Targets, Pos);
    if (End == string::npos)
      End = Tag.size();
    else if (Tag[End] == '/') {
      TagType = ETagType::Complete;
      break;
    }
    string Token = Tag.substr(Pos, End - Pos);

    if (First) {
      name = Token;
      if (!Token.empty()) {
        TagType = (name[0] == '/') ? ETagType::Close : ETagType::Open;
      }
      First = false;
    }
    else {
      size_t Split = Token.find(R"(=")", 0);
      CValue Attribute(Token.substr(0, Split), Token.substr(Split + 2, Token.size() - Split - 3));
      attributes.push_back(Attribute);
    }

    Pos = End + 1;
  }
  return TagType;
}

//------------------------------------------------------------------------------
// CNode::CNode(const string &name, const string &value)
//
//
//
CNode::CNode(const string &name, const string &value) : CValue(name, value) {
}

//------------------------------------------------------------------------------
// CNode::CNode(const CNode &other) 
//
//
//
CNode::CNode(const CNode &other) : CValue(other), mAttributes(other.mAttributes), mChildren(other.mChildren) {
}

//------------------------------------------------------------------------------
// CNode::CNode(CNode &&other)
//
//
//
CNode::CNode(CNode &&other) : CValue(other), mAttributes(other.mAttributes), mChildren(other.mChildren) {
}

//------------------------------------------------------------------------------
// CNode &CNode::operator=(const CNode &other)
//
//
//
CNode &CNode::operator=(const CNode &other) {
  mAttributes = other.mAttributes;
  mChildren = other.mChildren;
  return *this;
}

//------------------------------------------------------------------------------
// CNode &CNode::operator=(CNode &&other)
//
//
//
CNode &CNode::operator=(CNode &&other) {
  mAttributes = move(other.mAttributes);
  mChildren = move(other.mChildren);
  return *this;
}

//------------------------------------------------------------------------------
// void CNode::AddributeAdd(const CValue &value)
//
//
//
void CNode::AddributeAdd(const CValue &value) {
  mAttributes.push_back(value);
}

//------------------------------------------------------------------------------
// void CNode::AddributeAdd(const string &name, const string &value)
//
//
//
void CNode::AddributeAdd(const string &name, const string &value) {
  mAttributes.push_back(move(CValue(name, value)));
}

//------------------------------------------------------------------------------
// void CNode::ChildAdd(const CNode &node)
//
//
//
void CNode::ChildAdd(const CNode &node) {
  mChildren.push_back(node);
}

//------------------------------------------------------------------------------
// void CNode::ChildAdd(CNode &&node)
//
//
//
void CNode::ChildAdd(CNode &&node) {
  mChildren.push_back(node);
}

//------------------------------------------------------------------------------
//
//
//
//
void CNode::PathParse(const string &path, vector<string> &parsing) {
  parsing.clear();

  size_t Pos = 0;
  while (Pos < path.size()) {
    size_t End = path.find("/", Pos);
    if (End == string::npos)
      End = path.size();
    parsing.push_back(path.substr(Pos, End - Pos));
    Pos = End + 1;
  }
}

//------------------------------------------------------------------------------
// CNode *CNode::Path(const string &path, size_t ix)
//
//
//
CNode *CNode::Path(const string &path, size_t ix) {
  vector<string> ParsedPath;
  PathParse(path, ParsedPath);
  if (ParsedPath.size() != 0 && ParsedPath[0] == mName) {
    CNode *pNode = this;
    for (size_t Ix = 1; Ix < ParsedPath.size(); ++Ix) {
      bool Last = (Ix == ParsedPath.size() - 1);
      for (size_t IxChild = 0; IxChild < pNode->mChildren.size(); ++IxChild) {
        if (pNode->mChildren[IxChild].Name() == ParsedPath[Ix]) {
          if (!Last) {
            pNode = &pNode->mChildren[IxChild];
            break;
          }
          else if (ix-- == 0) {
            pNode = &pNode->mChildren[IxChild];
            return pNode;
          }
        }
      }
    }
  }

  return nullptr;
}

//------------------------------------------------------------------------------
// void CNode::Print(ostream &outStream, int level) const
//
//
//
void CNode::Print(ostream &outStream, int level) const {
  bool IsBranch = !mChildren.empty();
  string Indent(2 * level, ' ');
  outStream << Indent << '<' << mName;
  for (const CValue &Attribute : mAttributes)
    outStream << ' ' << Attribute.Name() << "=\"" << Attribute.Value() << "\"";
  if (IsBranch || !mValue.empty()) {
    outStream << '>';
    if (IsBranch)
      outStream << endl;
    for (const CNode &Node : mChildren)
      Node.Print(outStream, level + 1);
    outStream << EntityRefIns(mValue);
    EntityRefRem(EntityRefIns(mValue));
    if (IsBranch)
      outStream << Indent;
    outStream << "</" << mName << '>' << endl;
  }
  else
    outStream << " />" << endl;
}

//------------------------------------------------------------------------------
