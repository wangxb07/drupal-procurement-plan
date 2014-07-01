<?php

/**
 * @file
 * Definition of views_handler_field_node_link_edit.
 */

/**
 * Field handler to present a link node edit.
 *
 * @ingroup views_field_handlers
 */
class procurement_plan_handler_field_plan_link_edit extends procurement_plan_handler_field_plan_link {

  /**
   * Renders the link.
   */
  function render_link($plan, $values) {
    // Ensure user has access to edit this plan.
    if (!procurement_plan_access('edit', $plan)) {
      return;
    }

    $this->options['alter']['make_link'] = TRUE;
    $this->options['alter']['path'] = "procurement-plan/$plan->ppid/edit";
    $this->options['alter']['query'] = drupal_get_destination();

    $text = !empty($this->options['text']) ? $this->options['text'] : t('edit');
    return $text;
  }
}
