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


def main():
    a = Expression(4, None, Expression(2, "c", Expression(3, "a", Expression(2, "b", Expression(1, "a")))))
    print(a);
    a.multiply_scalar(5);
    print(a);
    a.simplify();
    print(a);

main();
