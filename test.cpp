#include <iostream>
#include <string>

class ExpressionNode {
public:
  ExpressionNode(double num = 0, std::string var = "", ExpressionNode* next = NULL) : m_num(num), m_var(var), m_next(next) {}
  
  std::string get_individual_display() {
    if (m_var.empty()) {
      return std::to_string(m_num);
    } else {
      if (m_num == 1) {
        return m_var;
      } else if (m_num == -1) {
        return "-" + m_var;
      } else {
        return std::to_string(m_num) + m_var;
      }
    }
  }

  ExpressionNode* m_next;
  double m_num;
  std::string m_var;
};

class Expression {
public:
  Expression(ExpressionNode* head = NULL) : m_head(head) {}
  
  std::string get_display() {
    
  }
  
  ExpressionNode* m_head;
};

int main() {
  ExpressionNode* a = new ExpressionNode(5, "a");
  
  std::cout << a->get_individual_display() << std::endl;
  return 0;
}
