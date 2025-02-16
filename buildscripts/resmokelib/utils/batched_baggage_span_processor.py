from typing import Optional
from opentelemetry.baggage import get_all as get_all_baggage
from opentelemetry.trace import Span
from opentelemetry.context import Context
from opentelemetry.sdk.trace.export import BatchSpanProcessor


class BatchedBaggageSpanProcessor(BatchSpanProcessor):
    """
    The BatchedBaggageSpanProcessor is combination between a baggage propagator and the BatchSpanProcessor.

    The BatchedBaggageSpanProcessor reads entries stored in Baggage
    from the parent context and adds the baggage entries' keys and
    values to the span as attributes on span start.

    Use this to propogate attributes you want in every nested span
    """

    def on_start(self, span: "Span", parent_context: Optional[Context] = None) -> None:
        baggage = get_all_baggage(parent_context)
        for key, value in baggage.items():
            span.set_attribute(key, value)
        super().on_start(span, parent_context)
