use egg::*;

define_language! {
    pub enum VecLang {
        Num(i32),

        // Id is a key to identify EClasses within an EGraph, represents
        // children nodes
        "+" = Add([Id; 2]),
        "*" = Mul([Id; 2]),
        "-" = Minus([Id; 2]),
        "<<" = Rot([Id; 2]),
        "-" = Neg([Id; 1]),
        // Vectors have width elements
        "Vec" = Vec(Box<[Id]>),

        // Vector operations that take 2 vectors of inputs
        "VecAdd" = VecAdd([Id; 2]),
        "VecMinus" = VecMinus([Id; 2]),
        "VecMul" = VecMul([Id; 2]),


        // Vector operations that take 1 vector of inputs
        "VecNeg" = VecNeg([Id; 1]),

        Symbol(egg::Symbol),
    }
}
#[derive(Default)]
pub struct ConstantFold;

pub type Egraph = egg::EGraph<VecLang, ConstantFold>;

impl Analysis<VecLang> for ConstantFold {
    type Data = Option<i32>;
    fn make(egraph: &Egraph, enode: &VecLang) -> Self::Data {
        let x = |i: &Id| egraph[*i].data.as_ref();
        Some(match enode {
            VecLang::Num(c) => *c,
            VecLang::Add([a, b]) => x(a)? + x(b)?,
            VecLang::Minus([a, b]) => x(a)? - x(b)?,
            VecLang::Mul([a, b]) => x(a)? * x(b)?,
            VecLang::Neg([a]) => -*x(a)?,
            VecLang::Rot([a, _b]) => *x(a)?,
            _ => return None,
        })
    }

    fn merge(&self, _to: &mut Self::Data, _from: Self::Data) -> bool {
        false
    }
}
