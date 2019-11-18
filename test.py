import copy;

class ExpressionNode: # DONE
    def __init__(self, num = 0, var = None, next = None):
        self.next = next;
        self.num = num;
        self.var = var;

    def get_individual_display(self): # DONE
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
    def __repr__(self): # USELESS
        return "ExprNode(" + self.get_individual_display() + ")";

class Expression:
    def __init__(self, head = None):
        self.head = head;


    def get_variables(self): # DONE
        """
        Gets list of variables names in expression
        """
        array = [];
        temp = self.head;
        
        while(temp != None):
            if (temp.var not in array):
                array.append(temp.var);
            temp = temp.next;
        return array;

    def get_value_for(self, var_name): # DONE
        """
        Get multiplier of specified variable
        """
        temp = self.head;
        
        while(temp != None):
            if (temp.var == var_name):
                return temp.num;
            temp = temp.next;
        return 0;

    def get_display(self): # DONE
        """
        Get a display for the whole expression (ex: 3a + 4b + -2c + 3)
        """
        out = "";
        temp = self.head;
        
        while(temp != None):
            out += temp.get_individual_display();
            if (temp.next != None):
                out += " + ";
            temp = temp.next;
        return out;

    def __repr__(self): # USELESS
        return "Expr(" + self.get_display() + ")";

    def multiply_scalar(self, scalar): # DONE
        """
        Multiply the expression by a scalar.
        """
        temp = self.head;
        
        while(temp != None):
            temp.num *= scalar;
            temp = temp.next;
        return self;

    def multiply(self, expression): # DONE
        """
        Multiply two expressions together
        (Warning: Throws exception when tryingo to do var * var )
        """
        temp = self.head;
        while(temp != None):
            current = expression.head;
            while (current != None):
                if (temp.var == None):
                    temp.num *= current.num;
                    temp.var= current.var;
                elif (current.var == None):
                    temp.num *= current.num;
                else:
                    raise Exception("Mashallah");
                current = current.next;
            temp = temp.next;
        return self;

    def add(self, expression): # DONE
        """
        Add an expression to another
        """
        temp = self.head;
        
        if (temp == None):
            self.head = expression.head;
            return self;
        
        while (temp.next != None):
            temp = temp.next;
        temp.next = expression.head;
        return self;

    def __simplify_add(self): # DONE
        temp = self.head;
        while(temp != None):
            current = temp.next;
            while (current != None):
                if (temp.var == current.var):
                    temp.num += current.num;
                    current.num = 0;
                    current.var = None;
                current = current.next;
            temp = temp.next;
        return self;
    
    def __simplify_remove_zero(self): # DONE
        temp = self.head;
        prev = None;
        
        while(temp != None):
            if (temp.num == 0):
                if (prev == None):
                    if (temp.next == None):
                        temp.num = 0;
                        temp.var = None;
                    else:
                        self.head = temp.next;
                else:
                    if (temp.next == None):
                        prev.next = None;
                    else:
                        prev.next = temp.next;
                    temp = prev;
                    prev = None;
                    continue;
            prev = temp;
            temp = temp.next;
        return self;
    
    def simplify(self): # DONE
        """
        Simplify the expression.
        """
        self.__simplify_add();
        self.__simplify_remove_zero();
        return self;

class ExpressionParser:
    @staticmethod
    def __split_numbers(text):
        number = "";
        var = "";
        var_name = False;
        for c in text:
            if (c in "-+0123456789." and var_name == False):
                number += c;
            else:
                var_name = True;
            if (var_name):
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
            num, var = ExpressionParser.__split_numbers(token);
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
            e = ExpressionNode(float(i[0]), i[1], e);
        return(Expression(e).simplify());
        

class ExpressionSolver:
    @staticmethod
    def __solve_matrix(equations):
        print(equations);
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
            index = 0;
            for i in range(len(equations)):
                if equations[i][0] != 0:
                    index = i;
                    break;
            #print "index "+str(eq)+": ",index
            #for the equation[index] calc the lists next itam  as follows
            lists.append([i * -1 / equations[index][0] for i in equations[index][1:]]);
            #print "list"+str(eq)+": ", lists[-1]
            #remove equation[index] and modify the others
            equations.pop(index);
            for i in equations:
                for j in range(len(lists[-1])):
                    i[j+1] = lists[-1][j] * i[0] + i[j+1];
                i.pop(0)

        lists.reverse();

        answers = [lists[0][0]];
        for i in range(1,len(lists)):
            tmpans = lists[i][-1]
            for j in range(len(lists[i])-1):
                tmpans += lists[i][j] * answers[-1-j]
            answers.append(tmpans)
        answers.reverse()
        return answers

    @staticmethod
    def solve(expression_list):
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
            matrix.append(individual.copy());
        
        return dict(zip(variables, ExpressionSolver.__solve_matrix(matrix)));   

def input_number_or_unknown(name, instead = None):
    val = input(name + ": ");
    if (val == "?"):
        return instead;
    else:
        return val;

def input_torsors():
    torsors = [];

    while True:
        name = input("name: ");
        if (name == ""):
            break;
        print("Position:");
        point_name = input("Point: ");
        point_x = ExpressionParser.parse(input_number_or_unknown("x", "x" + point_name));
        point_y = ExpressionParser.parse(input_number_or_unknown("y", "y" + point_name));
        point_z = ExpressionParser.parse(input_number_or_unknown("z", "z" + point_name));
        print("Composantes:");
        x = ExpressionParser.parse(input_number_or_unknown("x", "x" + name));
        y = ExpressionParser.parse(input_number_or_unknown("y", "y" + name));
        z = ExpressionParser.parse(input_number_or_unknown("z", "z" + name));
        l = ExpressionParser.parse(input_number_or_unknown("l", "l" + name));
        m = ExpressionParser.parse(input_number_or_unknown("m", "m" + name));
        n = ExpressionParser.parse(input_number_or_unknown("n", "n" + name));
        
        """
        Mo = Ma + OA ^ R
        
        yOA yR
        zOA zR
        xOA xR
        yOA yR
        
        x = yOA * zR - zOA * yR
        y = zOA * xR - xOA * zR
        z = xOA * yR - yOA * xR
        
        xMb = xMa + yOA * zR - zOA * yR
        yMb = yMa + zOA * xR - xOA * zR
        zMb = zMa + xOA * yR - yOA * xR
        
        """
        
        l.add(copy.deepcopy(point_y).multiply(z)).add(copy.deepcopy(point_z).multiply(y).multiply_scalar(-1));
        m.add(copy.deepcopy(point_z).multiply(x)).add(copy.deepcopy(point_x).multiply(z).multiply_scalar(-1));
        n.add(copy.deepcopy(point_x).multiply(y)).add(copy.deepcopy(point_y).multiply(x).multiply_scalar(-1));
        
        l.simplify();
        m.simplify();
        n.simplify();
        print(name, (x, y, z, l, m, n))
        torsors.append((x, y, z, l, m, n));
    return torsors;

def main():
    torsors = input_torsors();
    
    print();
    
    equations = [Expression(ExpressionNode(0)) for i in range(5)];
    
    for torsor in torsors:
        for i in range(5):
            equations[i].add(torsor[i]);
    
    for i in range(5):
        equations[i].simplify();
        print(equations[i]);
    
    print(ExpressionSolver.solve(equations));
    

main();
