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
        "VecAddRot" = VecAddRot([Id; 2]),
        "VecMinusRot" = VecMinusRot([Id; 2]),
        "VecMulRot" = VecMulRot([Id; 2]),
        Symbol(egg::Symbol),
    }
}

#[derive(Clone)]
#[derive(Default)]
pub struct ConstantFold; /* {
    pub blacklist_nodes : HashSet<Vec<Vec<VecLang>>>,
    pub precedent_map : HashMap<(VecLang, Id), HashSet<Id>>,

} */

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
            // VecAdd and similar operations return None to skip i32 representation
            VecLang::VecAdd(_) | VecLang::VecMul(_) | VecLang::Vec(_) | VecLang::VecMinus(_) | VecLang::VecNeg(_) | VecLang::VecAddRot(_) | VecLang::VecMinusRot(_) => return None,
            _ => return None,
        })
    }
  
    fn merge(&mut self, to: &mut Self::Data, from: Self::Data) -> DidMerge {
        egg::merge_max(to, from)
    }
    fn modify(egraph: &mut Egraph, id: Id) {
        let class = &mut egraph[id];
        if let Some(c) = class.data.clone() {
            let added = egraph.add(VecLang::Num(c.clone()));
            let merged = egraph.union(id, added);
            if merged {
                egraph.rebuild();
            }
            // to not prune, comment this out
            //egraph[id].nodes.retain(|n| n.is_leaf());

            // assert!(
            //     !egraph[id].nodes.is_empty(),
            //     "empty eclass! {:#?}",
            //     egraph[id]
            // );
            // #[cfg(debug_assertions)]
            // egraph[id].assert_unique_leaves();
        }
    }
}