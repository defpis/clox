#include "scanner.h"
#include "lox.h"

std::map<std::string, TokenType> Scanner::_keywords = {
    {"and", TokenType::AND},   {"class", TokenType::CLASS}, {"else", TokenType::ELSE},     {"false", TokenType::FALSE},
    {"for", TokenType::FOR},   {"fun", TokenType::FUN},     {"if", TokenType::IF},         {"nil", TokenType::NIL},
    {"or", TokenType::OR},     {"print", TokenType::PRINT}, {"return", TokenType::RETURN}, {"super", TokenType::SUPER},
    {"this", TokenType::THIS}, {"true", TokenType::TRUE},   {"var", TokenType::VAR},       {"while", TokenType::WHILE},
};

std::optional<TokenType> Scanner::keywordType(std::string &keyword) {
  auto it = _keywords.find(keyword);
  if (it != _keywords.end()) {
    return it->second;
  }
  return std::nullopt;
}

void Scanner::reset() {
  _code = "";
  _tokens.clear();
  _start = 0;
  _current = 0;
  _line = 1;
}

char Scanner::advance() {
  _current++;
  return _code.at(_current - 1);
}

bool Scanner::match(char c) {
  if (isAtEnd()) {
    return false;
  }
  if (_code.at(_current) != c) {
    return false;
  }
  _current++;
  return true;
}

char Scanner::peek() {
  if (isAtEnd()) {
    return '\0';
  }
  return _code.at(_current);
}

char Scanner::peekPrev() { return _code.at(_current - 1); }

char Scanner::peekNext() {
  if (_current + 1 >= _code.size()) {
    return '\0';
  }
  return _code.at(_current + 1);
}

bool Scanner::isAtEnd() { return _current >= _code.size(); }

bool Scanner::isDigit(char c) { return c >= '0' && c <= '9'; }

bool Scanner::isAlpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }

bool Scanner::isAlphaNumeric(char c) { return isAlpha(c) || isDigit(c); }

void Scanner::string(char c) {
  while ((peek() != c) && !isAtEnd()) {
    if (peek() == '\n') {
      _line++;
    }
    advance();
  }

  if (isAtEnd()) {
    lox::error(_line, "Unterminated string.");
    return;
  }

  advance(); // 跳过闭合的引号

  std::string literal = _code.substr(_start + 1, (_current - 1) - (_start + 1));
  addToken(TokenType::STRING, literal);
}

void Scanner::number() {
  while (isDigit(peek()) && !isAtEnd()) {
    advance();
  }

  if (peek() == '.' && isDigit(peekNext()) && !isAtEnd()) {
    advance();
    while (isDigit(peek()) && !isAtEnd()) {
      advance();
    }
  }

  double literal = std::stod(_code.substr(_start, _current - _start));
  addToken(TokenType::NUMBER, literal);
}

void Scanner::identifier() {
  while (isAlphaNumeric(peek()) && !isAtEnd()) {
    advance();
  }
  std::string keyword = _code.substr(_start, _current - _start);
  auto opt = keywordType(keyword);
  auto type = opt.has_value() ? opt.value() : TokenType::IDENTIFIER;
  addToken(type);
}

void Scanner::addToken(TokenType type) { addToken(type, nullptr); }

void Scanner::addToken(TokenType type, const std::any &literal) {
  std::string lexeme = _code.substr(_start, _current - _start);
  _tokens.emplace_back(std::make_shared<Token>(type, lexeme, literal, _line));
}

void Scanner::scanToken() {
  char c = advance();

  switch (c) {
    case '(': {
      addToken(TokenType::LEFT_PAREN);
      break;
    }
    case ')': {
      addToken(TokenType::RIGHT_PAREN);
      break;
    }
    case '{': {
      addToken(TokenType::LEFT_BRACE);
      break;
    }
    case '}': {
      addToken(TokenType::RIGHT_BRACE);
      break;
    }
    case ',': {
      addToken(TokenType::COMMA);
      break;
    }
    case '.': {
      addToken(TokenType::DOT);
      break;
    }
    case '-': {
      addToken(TokenType::MINUS);
      break;
    }
    case '+': {
      addToken(TokenType::PLUS);
      break;
    }
    case ';': {
      addToken(TokenType::SEMICOLON);
      break;
    }
    case '*': {
      addToken(TokenType::DOT);
      break;
    }
    case '!': {
      addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
      break;
    }
    case '=': {
      addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
      break;
    }
    case '<': {
      addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
      break;
    }
    case '>': {
      addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
      break;
    }
    case '/': {
      if (match('/')) {
        // 行注释
        while (peek() != '\n' && !isAtEnd()) {
          advance();
        }
      } else if (match('*')) {
        // 块注释
        while ((peekPrev() != '*' || peek() != '/') && !isAtEnd()) {
          if (peek() == '\n') {
            _line++;
          }
          advance();
        }

        if (isAtEnd()) {
          lox::error(_line, "Unclosed block comment.");
          return;
        }

        advance(); // 跳过后续的'/'
      } else {
        addToken(TokenType::SLASH);
      }
    }
    case ' ':
    case '\r':
    case '\t': {
      break;
    }
    case '\n': {
      _line++;
      break;
    }
    case '\'': {
      string('\'');
      break;
    }
    case '"': {
      string('"');
      break;
    }
    default: {
      if (isDigit(c)) {
        number();
      } else if (isAlpha(c)) {
        identifier();
      } else {
        lox::error(_line, "Unexpected character.");
      }
    }
  }
}

std::vector<SPToken> Scanner::scanTokens(const std::string &code) {
  reset();

  _code = code;

  while (!isAtEnd()) {
    _start = _current;
    scanToken();
  }

  _tokens.emplace_back(std::make_shared<Token>(Token::EOF_, "", nullptr, _line));

  return _tokens;
}
