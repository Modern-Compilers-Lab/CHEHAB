from langchain_openai import ChatOpenAI
from langchain.schema import StrOutputParser
from .prompt import build_veclang_prompt
from .config import GeneratorConfig

def build_chain(cfg: GeneratorConfig):
    prompt = build_veclang_prompt()
    llm = ChatOpenAI(
        model=cfg.llm_model,
        temperature=cfg.temperature,
        base_url=cfg.base_url,
        max_retries=cfg.max_retries,
    )
    parser = StrOutputParser()
    return prompt | llm | parser
