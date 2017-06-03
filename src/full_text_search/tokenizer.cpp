/***************************************************************************************************
 *
 * $QTCARTO_BEGIN_LICENSE:GPL3$
 *
 * Copyright (C) 2016 Fabrice Salvaire
 * Contact: http://www.fabrice-salvaire.fr
 *
 * This file is part of the QtCarto library.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $QTCARTO_END_LICENSE$
 *
 **************************************************************************************************/

/**************************************************************************************************/

#include "tokenizer.h"

#include "stop_words.cpp"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QTextBoundaryFinder>
#include <QtDebug>

/**************************************************************************************************/

Token::Token()
  : m_flags(0),
    m_token()
{}

Token::Token(const Token & other)
  : m_flags(other.m_flags),
    m_token(other.m_token)
{}

Token::Token(const QString & string)
  : Token()
{
  set(string); // Fixme: clear
}

Token::Token(const QStringRef & string)
  : Token()
{
  set(string); // Fixme: clear
}

Token::~Token()
{}

Token &
Token::operator=(const Token & other)
{
  if (this != &other) {
    m_flags = other.m_flags;
    m_token = other.m_token;
  }

  return *this;
}

Token &
Token::operator=(const QString & string)
{
  set(string);

  return *this;
}

Token &
Token::operator=(const QStringRef & string)
{
  set(string);

  return *this;
}

bool
Token::operator==(const Token & other) const
{
  return m_token == other.m_token;
}

void
Token::operator+=(const QChar & c)
{
  m_token += c;

  if (c.isLetter())
    m_flags |= Letter;
  else if (c.isNumber())
    m_flags |= Number;
  else {
    auto category = c.category();
    if (category == QChar::Punctuation_Dash)
      m_flags |= Hyphen;
    else
      m_flags |= Other;
  }
}

Token &
Token::operator<<(const QChar & c)
{
  *this += c;
  return *this;
}

void
Token::set(const QString & string)
{
  clear();
  for (const auto & c : string)
    *this += c;
}

void
Token::set(const QStringRef & string)
{
  clear();
  for (const auto & c : string)
    *this += c;
}

void
Token::clear()
{
  m_token.clear();
  m_flags = 0;
}

Token
Token::to_lower_case() const
{
   // Fixme: toCaseFolded, faster
  return Token(m_token.toLower());
}

Token
Token::to_upper_case() const
{
  return Token(m_token.toUpper());
}

Token
Token::ascii_folding() const
{
  QString decomposed_string = m_token.normalized(QString::NormalizationForm_D);
  Token output;
  for (const auto & c : decomposed_string)
    if (c.isLetterOrNumber())
      output += c;

  return output;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug
operator<<(QDebug debug, const Token & token)
{
  QDebugStateSaver saver(debug); // Fixme: ???

  debug << token.value();

  return debug;
}
#endif

/**************************************************************************************************/

TokenizedTextDocument::TokenizedTextDocument()
  : LocalizedDocument(),
    m_tokens()
{}

TokenizedTextDocument::TokenizedTextDocument(LanguageCode language)
  : LocalizedDocument(language),
    m_tokens()
{}

TokenizedTextDocument::TokenizedTextDocument(const TokenizedTextDocument & other)
  : LocalizedDocument(other),
    m_tokens(other.m_tokens)
{}

TokenizedTextDocument::~TokenizedTextDocument()
{}

TokenizedTextDocument &
TokenizedTextDocument::operator=(const TokenizedTextDocument & other)
{
  if (this != &other) {
    LocalizedDocument::operator=(other);
    m_tokens = other.m_tokens;
  }

  return *this;
}

bool
TokenizedTextDocument::operator==(const TokenizedTextDocument & other) const
{
  return LocalizedDocument::operator==(other) and
    m_tokens == other.m_tokens;
}

void
TokenizedTextDocument::append(const Token & token)
{
  if (token)
    m_tokens.append(token);
}

/**************************************************************************************************/

TokenizedTextDocument
WordTokenizer::process(const TextDocument & document) const
{
  // http://unicode.org/reports/tr29/ :  Unicode Standard Annex #29 — Unicode Text Segmentation

  const QString & input = document.document(); // Fixme: name
  TokenizedTextDocument output(document.language());
  QTextBoundaryFinder boundary_finder(QTextBoundaryFinder::Word, input);

  int index = 0;
  int next_index = -1;
  while ((next_index = boundary_finder.toNextBoundary()) != -1) {
    QStringRef word(&input, index, next_index - index);
    Token token(word);
    if (token.has_letter())
      output << token;
    index = next_index;
  }

  return output;
}

/**************************************************************************************************/

TokenizedTextDocument
WordFilterTraits::process(const TokenizedTextDocument & document) const
{
  TokenizedTextDocument output(document.language());

  for (const auto & token : document)
    output << process(token);

  return output;
}

/**************************************************************************************************/

StopWordFilter::StopWordFilter()
  : m_stop_words()
{}

StopWordFilter::StopWordFilter(const QStringList & words)
  : StopWordFilter()
{
  add_stop_words(words);
}

StopWordFilter::StopWordFilter(const QJsonArray & json_data)
{
  for (const auto & item : json_data)
    add_stop_word(item.toString());
}

StopWordFilter::~StopWordFilter()
{}

TokenizedTextDocument
StopWordFilter::process(const TokenizedTextDocument & document) const
{
  TokenizedTextDocument output(document.language());

  for (const auto & token : document)
    if (not is_stop_words(token))
        output << token;

  return output;
}

void
StopWordFilter::add_stop_word(const QString & word)
{
  // Fixme: ???
  QString without_accent = Token(word).ascii_folding().value();
  m_stop_words << word;
  m_stop_words << without_accent;
}

void
StopWordFilter::add_stop_words(const QStringList & words)
{
  for (const auto & word : words)
    add_stop_word(word);
}

void
StopWordFilter::set_stop_words(const QStringList & words)
{
  m_stop_words.clear();
  add_stop_words(words);
}

/**************************************************************************************************/

LanguageFilter::LanguageFilter()
  : m_filters()
{}

LanguageFilter::~LanguageFilter()
{}

LanguageFilter::FilterPtr
LanguageFilter::language_filter(const LanguageCode & language)
{
  return m_filters.contains(language) ?
    m_filters[language] : FilterPtr();
}

void
LanguageFilter::add_language_filter(const LanguageCode & language, const FilterPtr & filter)
{
  m_filters[language] = filter;
}

TokenizedTextDocument
LanguageFilter::process(const TokenizedTextDocument & document) const
{
  const LanguageCode language = document.language();
  if (m_filters.contains(language)) {
    FilterPtr filter = m_filters[language];
    // qInfo() << "LanguageFilter for " << language;
    return filter->process(document);
  } else
    return document;
}

/**************************************************************************************************/

Tokenizer::Tokenizer(WordTokenizer * word_tokenizer)
  : m_word_tokenizer(nullptr),
    m_filters()
{
  if (word_tokenizer == nullptr)
    m_word_tokenizer = WordTokenizerPtr(new WordTokenizer());
}

Tokenizer::Tokenizer(const Tokenizer & other)
  : m_word_tokenizer(other.m_word_tokenizer),
    m_filters(other.m_filters)
{}

Tokenizer::~Tokenizer()
{}

Tokenizer &
Tokenizer::operator=(const Tokenizer & other)
{
  if (this != &other) {
    m_word_tokenizer = other.m_word_tokenizer;
    m_filters = other.m_filters;
  }

  return *this;
}

void
Tokenizer::add_filter(TokenFilterTraits * filter)
{
  m_filters << FilterPtr(filter);
}

Tokenizer &
Tokenizer::operator<<(TokenFilterTraits * filter)
{
  add_filter(filter);
  return *this;
}

TokenizedTextDocument
Tokenizer::process(const TextDocument & document) const
{
  TokenizedTextDocument output = m_word_tokenizer->process(document);

  for (const auto & filter : m_filters)
    output = filter->process(output);

  return output;
}

/**************************************************************************************************/

Token
EnglishFilter::process(const Token & token) const
{
  if (token.has_other()) {
    const QString & input = token.value();
    QString output = strip_possessive(input);
    return Token(output);
  } else
    return token;
}

QString
EnglishFilter::strip_possessive(const QString & word) const
{
  // Strip right part before the first elision sign
  QChar form1 = '\''; // John's
  QChar form2 = 0x2019; // John’s RIGHT SINGLE QUOTATION MARK
  int position1 = word.lastIndexOf(form1);
  int position2 = word.lastIndexOf(form2);
  int position = qMin(position1, position2);
    if (position != -1)
      return word.left(position);
    else
      return word;
}

/**************************************************************************************************/

Token
FrenchFilter::process(const Token & token) const
{
  if (token.has_other()) {
    const QString & input = token.value();
    QString output = strip_elision(input);
    return Token(output);
  } else
    return token;
}

QString
FrenchFilter::strip_elision(const QString & word) const
{
  // Fixme: add part ???

  // Fixme: can normalize ’ to '
  // Strip left part before the last elision sign
  QChar form1 = '\''; // L'élision
  QChar form2 = 0x2019; // L’élision RIGHT SINGLE QUOTATION MARK
  int position1 = word.lastIndexOf(form1);
  int position2 = word.lastIndexOf(form2);
  int position = qMax(position1, position2);
  // qInfo() << "FrenchFilter elision" << word << position;
  if (position != -1)
    return word.mid(position);
  else
    return word;
}

/**************************************************************************************************/

PreLanguageFilter::PreLanguageFilter()
  : LanguageFilter()
{
  add_language_filter(LanguageCode::French, new FrenchFilter());
  add_language_filter(LanguageCode::English, new EnglishFilter());
}

/**************************************************************************************************/

LocalizedStopWordFilter::LocalizedStopWordFilter()
  : LanguageFilter()
{}

LocalizedStopWordFilter::LocalizedStopWordFilter(const QString & json_path)
  : LanguageFilter()
{
  QFile json_file(json_path);

  if (!json_file.open(QIODevice::ReadOnly))
    throw std::invalid_argument("Couldn't open file."); // Fixme: ???

  QByteArray json_data = json_file.readAll();
  QJsonParseError parse_error;
  QJsonDocument json_document = QJsonDocument::fromJson(json_data, &parse_error);
  if (parse_error.error == QJsonParseError::NoError) {
    // Keys are ISO 639-1 language code
    QJsonObject root = json_document.object();
    for (const auto & iso_language : root.keys()) {
      QLocale language(iso_language);
      FilterPtr filter(new StopWordFilter(root[iso_language].toArray()));
      add_language_filter(language.language(), filter);
    }
  }
  else
    qCritical() << parse_error.errorString();
}

/**************************************************************************************************/

StemmerFilter::StemmerFilter(const LanguageCode & language)
  : m_stemmer(language)
{}

Token
StemmerFilter::process(const Token & token) const
{
  QString output = m_stemmer.process(token.value());
  // qInfo() << token << "->" << output;
  return Token(output);
}

/**************************************************************************************************/

LocalizedStemmerFilter::LocalizedStemmerFilter()
  : LanguageFilter()
{
  for (const auto & language : Stemmer::available_languages()) {
    // qInfo() << "Add stemmer filter for" << language;
    add_language_filter(language, new StemmerFilter(language));
  }
}

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/