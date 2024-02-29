#include "scanner.h"
#include "lox.h"

std::map<std::string, TokenType> Scanner::keywords = {
    {"getter", TokenType::GETTER}, {"setter", TokenType::SETTER}, {"static", TokenType::STATIC},
    {"and", TokenType::AND},       {"class", TokenType::CLASS},   {"else", TokenType::ELSE},
    {"false", TokenType::FALSE},   {"for", TokenType::FOR},       {"fun", TokenType::FUN},
    {"if", TokenType::IF},         {"nil", TokenType::NIL},       {"or", TokenType::OR},
    {"print", TokenType::PRINT},   {"return", TokenType::RETURN}, {"super", TokenType::SUPER},
    {"this", TokenType::THIS},     {"true", TokenType::TRUE},     {"var", TokenType::VAR},
    {"while", TokenType::WHILE},
};

std::optional<TokenType> Scanner::keywordType(std::string &keyword) {
  auto it = keywords.find(keyword);
  if (it != keywords.end()) {
    return it->second;
  }
  return std::nullopt;
}

void Scanner::reset() {
  code = "";
  tokens.clear();
  start = 0;
  current = 0;
  line = 1;
}

char Scanner::advance() {
  current++;
  return code.at(current - 1);
}

bool Scanner::match(char c) {
  if (isAtEnd()) {
    return false;
  }
  if (code.at(current) != c) {
    return false;
  }
  current++;
  return true;
}

char Scanner::peek() {
  if (isAtEnd()) {
    return '\0';
  }
  return code.at(current);
}

char Scanner::peekPrev() { return code.at(current - 1); }

char Scanner::peekNext() {
  if (current + 1 >= code.size()) {
    return '\0';
  }
  return code.at(current + 1);
}

bool Scanner::isAtEnd() { return current >= code.size(); }

bool Scanner::isDigit(char c) { return c >= '0' && c <= '9'; }

bool Scanner::isAlpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }

bool Scanner::isAlphaNumeric(char c) { return isAlpha(c) || isDigit(c); }

void Scanner::string(char c) {
  while ((peek() != c) && !isAtEnd()) {
    if (peek() == '\n') {
      line++;
    }
    advance();
  }

  if (isAtEnd()) {
    lox::error(line, "Unterminated string.");
    return;
  }

  advance(); // 跳过闭合的引号

  std::string literal = code.substr(start + 1, (current - 1) - (start + 1));
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

  double literal = std::stod(code.substr(start, current - start));
  addToken(TokenType::NUMBER, literal);
}

void Scanner::identifier() {
  while (isAlphaNumeric(peek()) && !isAtEnd()) {
    advance();
  }
  std::string keyword = code.substr(start, current - start);
  std::optional<TokenType> opt = keywordType(keyword);
  TokenType type = opt.has_value() ? opt.value() : TokenType::IDENTIFIER;
  addToken(type);
}

void Scanner::addToken(TokenType type) { addToken(type, nullptr); }

void Scanner::addToken(TokenType type, const std::any &literal) {
  std::string lexeme = code.substr(start, current - start);
  tokens.push_back(std::make_shared<Token>(type, lexeme, literal, line));
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
    case ';': {
      addToken(TokenType::SEMICOLON);
      break;
    }
    case '-': {
      if (match('-')) {
        addToken(TokenType::MINUS_MINUS);
      } else if (match('=')) {
        addToken(TokenType::MINUS_EQUAL);
      } else {
        addToken(TokenType::MINUS);
      }
      break;
    }
    case '+': {
      if (match('+')) {
        addToken(TokenType::PLUS_PLUS);
      } else if (match('=')) {
        addToken(TokenType::PLUS_EQUAL);
      } else {
        addToken(TokenType::PLUS);
      }
      break;
    }
    case '*': {
      if (match('*')) {
        addToken(TokenType::STAR_STAR);
      } else if (match('=')) {
        addToken(TokenType::STAR_EQUAL);
      } else {
        addToken(TokenType::STAR);
      }
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
            line++;
          }
          advance();
        }

        if (isAtEnd()) {
          lox::error(line, "Unclosed block comment.");
          return;
        }

        advance(); // 跳过后续的'/'
      } else if (match('=')) {
        addToken(TokenType::SLASH_EQUAL);
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
      line++;
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
        std::string s(1, c);
        lox::error(line, "Unexpected character '" + s + "'.");
      }
    }
  }
}

std::vector<SPToken> Scanner::scanTokens(const std::string &_code) {
  reset();

  code = _code;

  while (!isAtEnd()) {
    start = current;
    scanToken();
  }

  tokens.push_back(std::make_shared<Token>(TokenType::EOF_, "", nullptr, line));

  return tokens;
}

Scanner &Scanner::getInstance() {
  static Scanner instance;
  return instance;
}
