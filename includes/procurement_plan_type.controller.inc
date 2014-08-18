<?php

/**
 * Procurement plan type controller class
 */
class ProcurementPlanTypeController extends EntityAPIControllerExportable {
   public function create(array $values = array()) {
    $values += array(
      'label' => '',
      'description' => '',
    );
    return parent::create($values);
  }

  /**
   * Save Task Type.
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    // attach default field to new product plan type.
    $transaction = isset($transaction) ? $transaction : db_transaction();
    $entity->is_new = !empty($entity->is_new) || empty($entity->{$this->idKey});
    if ($entity->is_new) {
      commerce_price_create_instance('procurement_plan_total', 'procurement_plan', $entity->type, t('Total Amount'));
    }
    // Rebuild menu registry. We do not call menu_rebuild directly, but set
    // variable that indicates rebuild in the end.
    // @see http://drupal.org/node/1399618
    variable_set('menu_rebuild_needed', TRUE);
    return parent::save($entity, $transaction);
  }
}

/**
 * UI controller for this controll.
 */
class ProcurementPlanTypeUIController extends EntityDefaultUIController {
  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['description'] = 'Manage Procurement plan types.';
    return $items;
  }
}

/**
 * Procurement Plan Type class.
 */
class ProcurementPlanType extends Entity {
  public $type;
  public $label;
  public $weight = 0;

  public function __construct($values = array()) {
    parent::__construct($values, 'procurement_plan_type');
  }

  function isLocked() {
    return isset($this->status) && empty($this->is_new) && (($this->status & ENTITY_IN_CODE) || ($this->status & ENTITY_FIXED));
  }
}
