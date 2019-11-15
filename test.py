class Expression:
    def __init__(self, num = 0, var = None, next = None):
        self.next = next;
        self.num = num;
        self.var = var;
        self.__already_simplified = False;
    
    def get_individual_display(self):
        """
        Get a display for the current symbol (ex: 3a).
        """
        if (self.var == None):
            return str(self.num);
        else:
            if(self.num == 1):
                return self.var;
            elif self.num == -1:
                return "-" + self.var;
            else:
                return str(self.num) + self.var;
        
    def get_display(self):
        """
        Get a display for the whole expression (ex: 3a + 4b + -2c + 3)
        """
        if (self.next == None):
            return self.get_individual_display();
        else:
            return self.next.get_display()  + " + " + self.get_individual_display();
    
    def __repr__(self):
        return self.get_display();
    
    def multiply_scalar(self, scalar):
        """
        Multiply the expression by a scalar.
        """
        self.num *= scalar;
        if (self.next == None):
            return self;
        else:
            return self.next.multiply_scalar(scalar);
    
    def __simplify_remove_zero(self, previous):
        """
        Remove part of expression where num = 0.
        """
        if (self.next == None):
            if (self.num == 0):
                if (previous != None):
                    previous.next = None;
            return self;
        else:
            if (self.num == 0):
                if (previous == None):
                    self.var = None;
                else:
                    self.previous.next = self.next;
        return self.next.__simplify_remove_zero(self);
        

    def __simplify__add(self):
        """
        Add duplicates variabes.
        """
        if (self.next == None):
            return self;
        
        current = self.next;
        while current != None:
            if self.var == current.var:
                self.num += current.num;
                current.num = 0;
            current = current.next;

        return self.next.simplify();
    
    def simplify(self):
        """
        Simplify the expression.
        """
        self.__simplify__add();
        self.__simplify_remove_zero(None);
    
    def add(self, to_add):
        """
        Add an expression to another
        """
        if self.next == None:
            self.next = to_add;
        else:
            self.next.add(to_add);
    
    @staticmethod
    def __split_numbers(text):
        number = "";
        var = "";
        for c in text:
            if (c in "-+0123456789."):
                number += c;
            else:
                var += c;
        return (number, var);
    
    @staticmethod
    def parse(text):
        """
        Parses a given expression
        """
        tokens = text.split("+");
        out = [];
        
        for token in tokens:
            token = token.replace(" ", "")
            num, var = Expression.__split_numbers(token);
            if (num == "-"):
                num = "-1";
            elif (num == ""):
                num = "1";
            elif (num == "+"):
                num = "1";
            if (var == ""):
                var = None;
            out.append((num, var));
        
        e = None;
        
        for i in out:
            e = Expression(float(i[0]), i[1], e);
        e.simplify();
        return(e);
        

def main():
    a = Expression.parse("3x + 5y + 6z + 34");
    b = Expression.parse("7x + y + 69z + 567");
    c = Expression.parse("-x + 32y + 2z + -1");
    print(a);
    print(b);
    print(c);

main();
