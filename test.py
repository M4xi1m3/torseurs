class Expression:
    def __init__(self, num = 0, var = None, next = None):
        self.next = next;
        self.num = num;
        self.var = var;
        self.__already_simplified = False;
    
    def replace_variables(self, dictio):
        for key, val in dictio.items():
            self.replace_variable(key, val);
    
    def replace_variable(self, var, value):
        if (self.var == var):
            self.var = None;
            self.num *= value;
        
        if (self.next == None):
            return self;
        else:
            return self.next.replace_variable(var, value);
    
    def get_variables(self, array = []):
        """
        Gets list of variables names in expression
        """
        if (self.var in array):
            pass;
        else:
            array.append(self.var);
        
        if (self.next == None):
            return array;
        else:
            return self.next.get_variables(array);
    
    def get_value_for(self, var_name):
        """
        Get multiplier of specified variable
        """
        if (self.var == var_name):
            return self.num;
        else:
            if (self.next == None):
                return 0;
            else:
                return self.next.get_value_for(var_name);
    
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

def solve_matrix(equations):
     #the constants of a system of linear equations are stored in a list for each equation in the system
     """
     for example the system below:
          2x+9y-3z+7w+8=0
          7x-2y+6z-1w-10=0
          -8x-3y+2z+5w+4=0
          0x+2y+z+w+0=0
     is expressed as the list:
          [[2,9,-3,7,8],[7,-2,6,-1,-10],[-8,-3,2,5,4],[0,2,1,1,0]]
     """
     lists=[] # I failed to name it meaningfully
     for eq in range(len(equations)):
          #print "equations 1", equations
          #find an equation whose first element is not zero and call it index
          index=0
          for i in range(len(equations)):
               if equations[i][0] != 0:
                    index=i;
                    break;
          #print "index "+str(eq)+": ",index
          #for the equation[index] calc the lists next itam  as follows
          lists.append([-1.0*i/equations[index][0] for i in equations[index][1:]])
          #print "list"+str(eq)+": ", lists[-1]
          #remve equation[index] and modify the others
          equations.pop(index)
          for i in equations:
               for j in range(len(lists[-1])):
                    i[j+1]+=i[0]*lists[-1][j]
               i.pop(0)

     lists.reverse()

     answers=[lists[0][0]]
     for i in range(1,len(lists)):
          tmpans=lists[i][-1]
          for j in range(len(lists[i])-1):
               tmpans+=lists[i][j]*answers[-1-j]
          answers.append(tmpans)
     answers.reverse()
     return answers

def slove(expression_list):
    variables = [];
    
    for expression in expression_list:
        variables.extend(x for x in expression.get_variables() if x not in variables);
    
    if (None in variables):
        variables.remove(None);
    variables.append(None);
    
    matrix = [];
    for expression in expression_list:
        individual = [];
        for var in variables:
            individual.append(expression.get_value_for(var));
        matrix.append(individual);
    
    return dict(zip(variables, solve_matrix(matrix)));    

def main():
    a = Expression.parse("1x + -1y +  2z + -5");
    b = Expression.parse("3x +  2y +  1z + -10");
    c = Expression.parse("2x + -3y + -2z + 10");
    values = slove([a, b, c]);
    
    a.replace_variables(values);
    a.simplify();
    print(a);
    b.replace_variables(values);
    b.simplify();
    print(b);
    c.replace_variables(values);
    c.simplify();
    print(c);

main();
