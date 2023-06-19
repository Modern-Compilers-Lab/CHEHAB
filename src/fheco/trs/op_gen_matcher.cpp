#include "fheco/trs/op_gen_matcher.hpp"

using namespace std;

namespace fheco::trs
{
std::size_t OpGenMatcher::count_ = 0;

size_t OpGenMatcher::HashRefWrapp::operator()(const OpGenMatcher::RefWrapp &matcher_ref) const
{
  return hash<OpGenMatcher>()(matcher_ref.get());
}

bool OpGenMatcher::EqualrRefWrapp::operator()(
  const OpGenMatcher::RefWrapp &lhs, const OpGenMatcher::RefWrapp &rhs) const
{
  return lhs.get() == rhs.get();
}
} // namespace fheco::trs
