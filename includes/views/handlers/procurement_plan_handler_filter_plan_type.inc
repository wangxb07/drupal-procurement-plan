<?php

/**
 * Filter by line item type
 */
class procurement_plan_handler_filter_plan_type extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Procurement plan');
      // TODO: get options
      $this->value_options = $options;
    }
  }
}
