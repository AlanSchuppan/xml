#include "stdafx.h"

#include <fstream>
#include <iostream>

#include "XML.h"

using namespace std;

//##############################################################################
// CTest
//##############################################################################
//##############################################################################

class CTest {
  string mName;
  vector<string> mNames;
  vector<CTest>  mTests;
public:
  CTest(const string &name, bool children = false);
  CTest(const CTest &other);
  CTest(CTest &&other);
  CTest &operator=(const CTest &other);
  CTest &operator=(CTest &&other);
};

CTest::CTest(const string &name, bool children) : mName(name) {
  mNames.push_back("Alan");
  mNames.push_back("Bob");
  mNames.push_back("Carl");
  if (children) {
    mTests.push_back(CTest("TestA"));
    mTests.push_back(CTest("TestB"));
    mTests.push_back(CTest("TestC"));
  }
}

CTest::CTest(const CTest &other) : mName(other.mName), mNames(other.mNames), mTests(other.mTests) {
}

CTest::CTest(CTest &&other) : mName(other.mName), mNames(other.mNames), mTests(other.mTests) {
}

CTest &CTest::operator=(const CTest &other) {
  mName = other.mName;
  mNames = other.mNames;
  mTests = other.mTests;
  return *this;
}

CTest &CTest::operator=(CTest &&other) {
  mName = move(other.mName);
  mNames = move(other.mNames);
  mTests = move(other.mTests);
  return *this;
}

//##############################################################################

int main() {
  char *Lyricalness[] = {
    "This is the <very> first line",
    "Number two and I'm still feeling fine",
    "Number three and a pattern begins",
    "<Add& lines like this and everyone wins!"
  };
  static const uint32_t NLines = sizeof(Lyricalness) / sizeof(*Lyricalness);

  //CTest Test("Test", true);
  //CTest Test2(Test);
  //CTest Test3(move(Test));
  //Test = Test3;
  //Test2 = move(Test3);

  CNode Song("song");
  {
    CValue Attribute("Test", "Passed");
    Song.AddributeAdd(Attribute);
  }

  {
    CNode Attributes("attributes");
    Attributes.AddributeAdd("color", "blue");
    Attributes.AddributeAdd(CValue("size", 16));
    //Attributes.AddributeAdd("size", "16");
    Song.ChildAdd(Attributes);
  }

  {
    CNode Title("title", "Song Title");
    Song.ChildAdd(move(Title));
  }

  {
    CNode Lyrics("lyrics", "Test how combination of children and value is handled.");
    for (uint32_t Ix = 0; Ix < NLines; ++Ix) {
      CNode Line("line", Lyricalness[Ix]);
      Line.AddributeAdd(CValue("index", Ix));
      //Line.AddributeAdd("index", to_string(Ix));
      Lyrics.ChildAdd(Line);
    }
    //{
    //  CNode Spoiler("spoiler", "This is a test.");
    //  Lyrics.ChildAdd(Spoiler);
    //}
    //for (uint32_t Ix = 0; Ix < NLines; ++Ix) {
    //  CNode Line("line", Lyricalness[Ix]);
    //  Line.AddributeAdd("index", to_string(Ix));
    //  Lyrics.ChildAdd(Line);
    //}
    Song.ChildAdd(Lyrics);
  }

  Song.Print(cout);
  //{
  //  ofstream OutFileStream;
  //  OutFileStream.open(R"(d:\PrgVSCpp\XML\XML\Debug\File.xml)", ofstream::out);
  //  Song.Print(OutFileStream);
  //  OutFileStream.close();
  //}
  {
    ifstream InFileStream;
    InFileStream.open(R"(d:\PrgVSCpp\XML\XML\Debug\File.xml)", ifstream::in);
    string Name;
    vector<CValue> Attributes;
    for (uint32_t Ix = 0; Ix < 7; ++Ix) {
      ETagType TagType = CNode::NextTag(InFileStream, Name, Attributes);
      TagType = ETagType::Unknown;
    }
    InFileStream.close();
  }

  for (size_t Ix = 0; Ix < 5; ++Ix) {
    CNode *pNode = Song.Path("song/lyrics/line", Ix);
    pNode = nullptr;
  }

  getchar();
  return 0;
}

