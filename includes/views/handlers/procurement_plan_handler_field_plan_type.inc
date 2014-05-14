<?php

/**
 * Field handler to translate a line item type into its readable form.
 */
class procurement_plan_handler_field_plan_type extends views_handler_field {
  public function render($values) {
    $type = $this->get_value($values);
    // TODO get type output value
    return $this->sanitize_value($value);
  }
}
